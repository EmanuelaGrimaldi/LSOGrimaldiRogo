#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utente.h"
#include "libro.h"
#include "carrello.h"
#include "server.h"
#include "define.h"

char *conninfo = "host=postgres-db port=5432 dbname=mydb user=myuser password=mypassword";
char *parolaChiave, *request, *email, *password, *nome, *bufferPointer, *charPointerISBN, *client_message, emailFinale[MAX_LENGTH];
char *bufferDeluxeDue, *charPointerK;
int ISBN, risultato, Kvalue;
int * intPointer;
char buffer[MAX_MESSAGE_LENGTH];

int main()
{
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    parolaChiave = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    email = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    password = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    nome = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    request = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    bufferPointer = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    charPointerISBN = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    client_message = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    bufferDeluxeDue = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char)*10);
    charPointerK = (char*)malloc(MAX_MESSAGE_LENGTH); 

    // Creazione socket server
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("Server: Errore durante la creazione della socket.\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // Bind
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Errore col comando 'Bind' in server.c\n");
        return 1;
    }

    // Ascolto
    listen(server_sock, 3);
    printf("Server in ascolto sulla porta 8080!♥\n");

    c = sizeof(struct sockaddr_in);
    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c);

        if (client_sock == -1){
            perror("Errore nell'acceept del client_sock.\n");
            continue;
        }

        //Qui è dove grazie al fork gestisco i multiprocessi

        pid_t pid = fork();

        if (pid < 0 ){

            perror("Errore fork");
            close(client_sock);
            continue;

        } else if ( pid == 0 ){

            //FIGLIO
            printf("PROCESSO FIGLIO %d :Client connesso con successo!♥\n", getpid());
            close(server_sock);
            handleClient(client_sock);
            close(client_sock);         // Chiude il client_socket al termine
            exit(0); 

        } else {
            //PADRE
            close(client_sock);
        }

        //Pulizia di eventuali processi zombie
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }

    close(server_sock);
    return 0;
}


// Funzione per leggere dalla socket fino al carattere '\n'
int leggi_fino_a_newline(int socket, char *buffer) {

    size_t i = 0;
    char c;
    int n;

    while (i < MAX_MESSAGE_LENGTH -1) {

        n = recv(socket, &c, 1, 0);  // Legge un carattere alla volta

        if (n > 0) {

            if ( c == '\n' ) {
                buffer[i] = '\0';  // Termina la stringa con sto coso
                return i;
            }
            buffer[i] = c;  // Aggiunge il carattere al buffer
            i++;
        } else if (n == 0) {
            printf("\nLa connessione è stata chiusa dal client.\n");
            // Connessione chiusa dal client
            break;
        } else {
            // Errore nella ricezione
            perror("errore in leggi_fino_a_newline: recv");
            return -1;
        }
    }
    buffer[i] = '\0';
    return i;
}

void handleClient(int socket)
{
    int read_size;

    printf("\nsono in handleClient, contenuto socket: %d \n\n", socket);

    memset(client_message, 0, MAX_MESSAGE_LENGTH);

    while ((read_size = leggi_fino_a_newline(socket, client_message)) > 0)
    {
        printf("\n\nmessaggio client ricevuto: %s\n\n", client_message);

        // OPZIONE REGISTRA: prendo dati utente, verifico che non esiste già l'email nel db e in caso registro.
        if (strcmp(client_message, "REGISTER") == 0)
        {
            //Attendo il buffer con tutti i dati          
            bzero(request, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            //divido il buffer in 3 variabili
            sscanf(buffer, "%[^;];%[^;];%[^;]", nome, email, password);

            if (emailValida(email, conninfo) == RISPOSTA_VALIDA)
            {
                registraNuovoUtente(socket, nome, email, password, conninfo);
                send(socket, "\n\nUtente registrato correttamente!\n\n", strlen("\n\nUtente registrato correttamente!\n\n"), 0);
            }
            else
            {
                send(socket, "\n\nNon è stato possibile registrare il nuovo utente.\n\n", strlen("\n\nNon è stato possibile registrare il nuovo utente.\n\n"), 0);
            }

        }


        // OPZIONE LOGIN: Prendo dati da utente e, se corrispondono nel DB, login.
        else if (strcmp(client_message, "LOGIN") == 0)
        { 
            //Attendo il buffer con tutti i dati          
            bzero(request, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            //divido il buffer in 2 variabili
            sscanf(buffer, "%[^;];%[^;]", email, password);

            //verifico prima se è admin
            if ( (strcmp(email, "admin") == 0) && (strcmp(password, "admin") == 0) )
            {

                send(socket, "ADMIN", strlen("ADMIN") + 1, 0);   
            } 
            else 
            {
                //se non è admin, verifica delle credenziali
                if (loginUtente(socket, email, password, conninfo) == RISPOSTA_VALIDA) {
                    send(socket, "RISPOSTA_VALIDA", strlen("RISPOSTA_VALIDA"), 0);

                } else {
                    send(socket, "RISPOSTA_INVALIDA", strlen("RISPOSTA_INVALIDA"), 0);

                }
            }

        }


        // OPZIONE RICERCA PAROLA CHIAVE: prendo la parola e vedo se ci sono similitudini, se si, la funzione cercaLibroByTitolo stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_PAROLACHIAVE") == 0)
        {
            //Attendo il buffer con la parola chiave         
            bzero(request, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            bufferPointer = cercaLibroByParolaChiave(socket, buffer, conninfo);

            send(socket, bufferPointer, strlen(bufferPointer), 0);
        }

        // OPZIONE RICERCA ISBN: prendo l'isbn e vedo se ci sono similitudini, se si, la funzione cercaLibroByISBN stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_ISBN\0") == 0)
        {
            //Attendo il buffer con l'ISBN        
            bzero(request, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            bufferPointer = cercaLibroByISBN(socket, buffer, conninfo);

            send(socket, bufferPointer, strlen(bufferPointer), 0);
        }

        // OPZIONE AGGIUNGERE AL CARRELLO: Funziona SOLO tramite ISBN.
        else if (strcmp(client_message, "ADD_TO_CART") == 0)
        {
           //Attendo il buffer con l'ISBN         
            bzero(buffer, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            risultato = aggiungiLibroAlCarrello(socket, email, buffer, conninfo); //"isLibroDisponibile" è implementata in carrello.c

            if (risultato == 1 ){
                send(socket, "\n\nLibro aggiunto con successo!\n\n", strlen("\n\nLibro aggiunto con successo!\n\n"), 0);

            } else if ( risultato == 2 ){
                send(socket, "\nSoglia massima di libri nel carrello raggiunta.\nSi prega di effettuare il checkout.\n", strlen("\nSoglia massima di libri nel carrello raggiunta.\nSi prega di effettuare il checkout.\n"), 0);

            } else {
                send(socket, "\n\nNon vi sono copie attualmente disponibili per questo libro.\n\n", strlen("\n\nNon vi sono copie attualmente disponibili per questo libro.\n\n"), 0);                
            }
        }

        else if (strcmp(client_message, "CHECKOUT") == 0)
        {
            //Attendo il buffer con l'email del checkout         
            bzero(buffer, MAX_MESSAGE_LENGTH);
            recv(socket, buffer, sizeof(buffer), 0);

            bufferPointer = checkout(socket, email, conninfo);

            if ( strcmp(bufferPointer, "") == 0){
                    send(socket, "Tutti i libri sono stati presi correttamente in prestito!\n", strlen("Tutti i libri sono stati presi correttamente in prestito!\n"), 0);
                } else {
                    send(socket, buffer, strlen(buffer), 0);
                }
        }

        else if (strcmp(client_message, "ELENCO_LIBRI") == 0)
        {
            bzero(bufferPointer, MAX_MESSAGE_LENGTH);
            bufferPointer = getAllLibri(conninfo);
            send(socket, bufferPointer, strlen(bufferPointer), 0);

        }

        else if (strcmp(client_message, "ELENCO_PRESTITI") == 0)
        {
            printf("\n1");
            bzero(bufferDeluxeDue, MAX_MESSAGE_LENGTH*sizeof(char)*9);
            printf("\n2");
            bufferDeluxeDue = getAllPrestiti(conninfo);
            printf("\n3");

            printf("\n\nIN SERVER.C - GET ALL PRESTITI:\n%s",bufferDeluxeDue);

            send(socket, bufferDeluxeDue, strlen(bufferPointer), 0);

        } 
        
        else if (strcmp(client_message, "MODIFICA_K") == 0)
        {
            //Restituisco K attuale
            int K_attuale = getValoreK(conninfo);
            snprintf(charPointerK, sizeof(charPointerK), "%d", K_attuale);
            printf("\nSono in server.c, Char K : %s\n", charPointerK);
            send(socket, charPointerK, strlen(charPointerK), 0);

            //Ricevo risposta se voglio cambiarlo
            bzero(buffer, MAX_MESSAGE_LENGTH);
            recv(socket, bufferPointer, MAX_MESSAGE_LENGTH, 0);

            char *endptr;  // Puntatore per verificare eventuali errori
            int rispostaK = bufferPointer[0] - '0';     

            if ( rispostaK > 0 ) {
                updateValoreK(conninfo, rispostaK);
                send(socket, "\nValore di K aggiornato correttamente.\n", strlen("\nValore di K aggiornato correttamente.\n"), 0);

            } else {
                send(socket, "\nValore di K rimasto invariato.\n", strlen("\nValore di K rimasto invariato.\n"), 0);
            }
        }
        else
        {
            send(socket, "ERRORE SERVER:Comando non valido.\n\n", strlen("ERRORE SERVER:Comando non valido.\n\n") + 1, 0);
        }
    }

    if (read_size == 0)
    {
        printf("Client disconnesso\n");
    }
    else if (read_size == -1)
    {
        perror("Errore con Recv in server.c\n");
    }
}