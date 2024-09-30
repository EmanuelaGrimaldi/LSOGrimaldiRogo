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

int main()
{
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    // Creazione socket server
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("Errore durante la creazione della socket in server.c\n");
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
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        printf("Client connesso con successo\n");
        handleClient(client_sock);
        close(client_sock);
    }

    if (client_sock < 0)
    {
        printf("Errore con 'accept' del client in server.c\n");
        return 1;
    }

    close(server_sock);
    return 0;
}

void handleClient(int socket)
{
    char client_message[MAX_MESSAGE_LENGTH];
    char *name, *email, *password, *parolaChiave, *titoloLibro;
    int read_size, isbn;

    while ((read_size = recv(socket, client_message, MAX_MESSAGE_LENGTH, 0)) > 0)
    {
        client_message[read_size] = '\0';

        // OK!!!!
        if (strcmp(client_message, "REGISTER") == 0)
        {
            recv(socket, name, 100, 0);
            recv(socket, email, 100, 0);
            recv(socket, password, 30, 0);

            if (emailValida(email) == RISPOSTA_VALIDA)
            {
                registraNuovoUtente(socket, name, email, password);
                send(socket, "Utente registrato correttamente!", strlen("Utente registrato correttamente!"), 0);
            }
            else
            {
                send(socket, "Non è stato possibile registrare il nuovo utente.", strlen("Non è stato possibile registrare il nuovo utente."), 0);
            }
        }

        // OK!!
        else if (strcmp(client_message, "LOGIN") == 0)
        {
            recv(socket, email, MAX_LENGTH, 0);
            recv(socket, password, MAX_LENGTH, 0);

            if (loginUtente(socket, email, password) == RISPOSTA_VALIDA)
            {
                accedi(email);
                send(socket, "ok", strlen("ok"), 0);
            }
            else
            {
                disAccedi();
                send(socket, "Non ok.", strlen("Non ok."), 0);
            }
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------DA TESTARE
        else if (strcmp(client_message, "SEARCH_BY_PAROLACHIAVE") == 0)
        {
            recv(socket, parolaChiave, MAX_LENGTH, 0);
            cercaLibroByTitolo(socket, parolaChiave);
            // restituisco info libro trovato?
        }
        else if (strcmp(client_message, "SEARCH") == 0)
        {
            recv(socket, titoloLibro, MAX_LENGTH, 0);
            cercaLibroByTitolo(socket, titoloLibro); // questa f stampa, non restituisce
            // restituisco info libro trovato?
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------DA TESTARE
        else if (strcmp(client_message, "SEARCH_BY_ISBN") == 0)
        {
            recv(socket, &isbn, MAX_LENGTH, 0);
            cercaLibroByISBN(socket, isbn);
            // restituisco info libro trovato?
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------PROBABLY OK
        else if (strcmp(client_message, "ADD_TO_CART") == 0)
        {
            recv(socket, &isbn, MAX_LENGTH, 0);
            aggiungiLibroAlCarrello(socket, email, isbn);
        }

        //------------------------------------------------------------------------------------------------------------------------------------------------TODO
        else if (strcmp(client_message, "CHECKOUT") == 0)
        {
            recv(socket, email, MAX_LENGTH, 0);
            checkout(socket, email);
        }
        else
        {
            send(socket, "Comando non valido", strlen("Comando non valido"), 0);
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
