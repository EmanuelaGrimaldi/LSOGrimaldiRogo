#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "utente.h"
#include "libro.h"
#include "carrello.h"
#include "server.h"
#include "define.h"

char *conninfo = "host=postgres-db port=5432 dbname=mydb user=myuser password=mypassword";
char *parolaChiave, *request, *email, *password, *nome, *bufferPointer, *charPointerISBN, *client_message;
int ISBN;
char buffer[MAX_MESSAGE_LENGTH];

int main()
{
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    parolaChiave = (char*)malloc(15*sizeof(char));
    email = (char*)malloc(25*sizeof(char));
    password = (char*)malloc(15*sizeof(char));
    nome = (char*)malloc(25*sizeof(char));
    request = (char*)malloc(512*sizeof(char));
    bufferPointer = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    charPointerISBN = (char*)malloc(10*sizeof(char));
    client_message = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));

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
        if (client_sock > -1)
        {
            // dovrebbe partire una processo per gestire l'handle, ma poi ci pensiamo
            printf("Client connesso con successo\n");
            handleClient(client_sock);
            close(client_sock);
        }
        else
        {
            perror("Server: Errore nell'accettazione della connessione.");
        }
    }

    if (client_sock < 0)
    {
        printf("Server: Errore con 'accept' del client.\n");
        return 1;
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
                printf("\nBUFFER FINALE IN SERVER: %s", buffer);
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
            // Chiede il nome completo
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci il tuo nome completo: ");
            send(socket, request, strlen(request), 0);
            recv(socket, nome, 25*sizeof(char), 0);

            // Chiede l'email
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci la tua email: ");
            send(socket, request, strlen(request), 0);
            recv(socket, email, 25*sizeof(char), 0);

            // Chiede la password
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci la tua password: ");
            send(socket, request, strlen(request), 0);
            recv(socket, password, 15*sizeof(char), 0);

            if (emailValida(email, conninfo) == RISPOSTA_VALIDA)
            {
                registraNuovoUtente(socket, nome, email, password, conninfo);
                send(socket, "\n\nUtente registrato correttamente!\n\n", strlen("\n\nUtente registrato correttamente!\n\n"), 0);
            }
            else
            {
                send(socket, "\n\nNon è stato possibile registrare il nuovo utente.\n\n", strlen("\n\nNon è stato possibile registrare il nuovo utente.\n\n"), 0);
            }

            memset(client_message, 0, MAX_MESSAGE_LENGTH);
        }


        // OPZIONE LOGIN: Prendo dati da utente e, se corrispondono nel DB, login.
        else if (strcmp(client_message, "LOGIN") == 0)
        {

            // Chiede l'email
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci la tua email: ");
            send(socket, request, strlen(request), 0);
            recv(socket, email, 25*sizeof(char), 0);

            // Chiede la password
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci la tua password: ");
            send(socket, request, strlen(request), 0);
            recv(socket, password, 15*sizeof(char), 0);

            printf("\nSERVER: riceve %s e %s\n", email, password);

            //verifica delle credenziali
            if (loginUtente(socket, email, password, conninfo) == RISPOSTA_VALIDA) {
                send(socket, "RISPOSTA_VALIDA", strlen("RISPOSTA_VALIDA"), 0);
            } else {
                send(socket, "RISPOSTA_INVALIDA", strlen("RISPOSTA_INVALIDA"), 0);
            }

            memset(client_message, 0, MAX_MESSAGE_LENGTH);
        }


        // OPZIONE RICERCA PAROLA CHIAVE: prendo la parola e vedo se ci sono similitudini, se si, la funzione cercaLibroByTitolo stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_PAROLACHIAVE") == 0)
        {
            // Chiede la parola chiave
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci la parola chiave: ");
            send(socket, request, strlen(request), 0);
            recv(socket, parolaChiave, 15*sizeof(char), 0);

            bufferPointer = cercaLibroByTitolo(socket, parolaChiave, conninfo);

            send(socket, bufferPointer, strlen(bufferPointer), 0);
            memset(client_message, 0, MAX_MESSAGE_LENGTH);
        }

        // OPZIONE RICERCA ISBN: prendo l'isbn e vedo se ci sono similitudini, se si, la funzione cercaLibroByISBN stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_ISBN\0") == 0)
        {
            // Chiede l'isbn'
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci l'ISBN: ");
            send(socket, request, strlen(request), 0);

            // Riceve isbn dal client
            recv(socket, charPointerISBN, 10*sizeof(char), 0);
            ISBN = atoi(charPointerISBN);

            bufferPointer = cercaLibroByISBN(socket, ISBN, conninfo);

            send(socket, bufferPointer, strlen(bufferPointer), 0);
            memset(client_message, 0, MAX_MESSAGE_LENGTH);
        }

        // OPZIONE AGGIUNGERE AL CARRELLO: Funziona SOLO tramite ISBN.
        else if (strcmp(client_message, "ADD_TO_CART") == 0)
        {
            memset(request, 0, sizeof(request));
            strcpy(request, "Inserisci l'ISBN: ");
            send(socket, request, strlen(request), 0);
            recv(socket, charPointerISBN, 10*sizeof(char), 0);

            ISBN = atoi(charPointerISBN);

            aggiungiLibroAlCarrello(socket, email, ISBN, conninfo); //"isLibroDisponibile" è implementata in carrello.c

            send(socket, "\n\nLibro aggiunto con successo!\n\n", strlen("\n\nLibro aggiunto con successo!\n\n"), 0);
            memset(client_message, 0, MAX_MESSAGE_LENGTH);
        }

        else if (strcmp(client_message, "CHECKOUT") == 0)
        {
            // Riceve l'email dal client
            recv(socket, email, 25*sizeof(char), 0);

            checkout(socket, email, conninfo);

            send(socket, "Prestito avvenuto correttamente!", strlen("Prestito avvenuto correttamente!"), 0);
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