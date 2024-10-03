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

const char *conninfo = "host=localhost port=5432 dbname=mydb user=myuser password=mypassword";
char *nome, *email, *password;

int main()
{
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

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

void handleClient(int socket)
{
    char client_message[MAX_MESSAGE_LENGTH];
    char *parolaChiave, *titoloLibro;
    int read_size, isbn;

    printf("\nsono in handleClient, contenuto socket: %d \n\n", socket);

    memset(client_message, 0, MAX_MESSAGE_LENGTH);

    while ((read_size = recv(socket, client_message, MAX_MESSAGE_LENGTH, 0)) > 0)
    {
        printf("\n\nmessaggio client ricevuto: %s\n\n", client_message);

        // OPZIONE REGISTRA: prendo dati utente, verifico che non esiste già l'email nel db e in caso registro.
        if (strcmp(client_message, "REGISTER") == 0)
        {
            printf("Inserisci qui il tuo nome per intero: ");
            scanf("%s", nome);
            printf("Inserisci qui la tua email: ");
            scanf("%s", email);
            printf("Inserisci qui la tua password: ");
            scanf("%s", password);

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

            printf("Inserisci la tua email: ");
            scanf("%s", email);
            printf("Inserisci qui la tua password: ");
            scanf("%s", password);

            if (loginUtente(socket, email, password, conninfo) == RISPOSTA_VALIDA)
            {
                accedi(email, conninfo);
                send(socket, "\n\nAccesso eseguito correttamente!\n\n", strlen("\n\nAccesso eseguito correttamente!\n\n"), 0);
            }
            else
            {
                logout();
                send(socket, "\n\nProblemi con l'accesso.\n\n", strlen("\n\nProblemi con l'accesso.\n\n"), 0);
            }
        }

        // OPZIONE RICERCA PAROLA CHIAVE: prendo la parola e vedo se ci sono similitudini, se si, la funzione cercaLibroByTitolo stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_PAROLACHIAVE") == 0)
        {
            printf("Inserisci la parola chiave per la ricerca del libro: ");
            scanf("%s", parolaChiave);
            cercaLibroByTitolo(socket, parolaChiave, conninfo);

            send(socket, "\n\nRicerca eseguita con successo!\n\n", strlen("\n\nRicerca eseguita con successo!\n\n"), 0);
        }

        // OPZIONE RICERCA ISBN: prendo l'isbn e vedo se ci sono similitudini, se si, la funzione cercaLibroByISBN stamperà tutte le occorrenze.
        else if (strcmp(client_message, "SEARCH_BY_ISBN") == 0)
        {
            printf("Inserisci l'ISBN per la ricerca del libro: ");
            scanf("%d", &isbn);
            cercaLibroByISBN(socket, isbn, conninfo);

            send(socket, "\n\nRicerca eseguita con successo!\n\n", strlen("\n\nRicerca eseguita con successo!\n\n"), 0);
        }

        // OPZIONE AGGIUNGERE AL CARRELLO: Funziona SOLO tramite ISBN.
        else if (strcmp(client_message, "ADD_TO_CART") == 0)
        {
            printf("Inserisci l'ISBN del libro da aggiungere al carrello: ");
            scanf("%d", &isbn);
            aggiungiLibroAlCarrello(socket, email, isbn, conninfo); //"isLibroDisponibile" è implementata in carrello.c

            send(socket, "\n\nLibro aggiunto con successo!\n\n", strlen("\n\nLibro aggiunto con successo!\n\n"), 0);
        }

        else if (strcmp(client_message, "CHECKOUT") == 0)
        {
            checkout(socket, email, conninfo);
            send(socket, "\n\nFine checkout!\n\n", strlen("\n\nFine checkout!\n\n"), 0);
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