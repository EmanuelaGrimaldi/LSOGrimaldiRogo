#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#include "utente.h"
#include "define.h"

int client_connesso;
char *user_name, *user_email;

int main()
{
    client_connesso = NON_CONNESSO;
    user_name = NULL;
    user_email = NULL;

    int socket_desc;
    struct sockaddr_in server;
    char message[MAX_MESSAGE_LENGTH], server_reply[MAX_MESSAGE_LENGTH];

    // Creazione socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Creazione socket fallita.\n");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    // Connessione al server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connessione al server fallita\n");
        return 1;
    }

    printf("Connessione al server avvenuta con successo!\n");
    menuGuest(socket_desc);
    close(socket_desc);
    return 0;
}

void menuGuest(int socket)
{
    int choice;
    while (1)
    {
        printf("\n--- Menu ---\n");
        printf("1. Registrazione nuovo utente\n");
        printf("2. Login\n");
        printf("3. Ricerca di un libro tramite parola chiave\n");
        printf("4. Ricerca di un libro tramite ISBN\n");
        printf("5. Esci\n");
        printf("Inserisci qui la tua opzione: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            send(socket, "REGISTER", strlen("REGISTER"), 0);
            break;
        case 2:
            send(socket, "LOGIN", strlen("LOGIN"), 0);

            if (recv(socket, "\n\nUtente registrato correttamente!\n\n", strlen("\n\nUtente registrato correttamente!\n\n"), 0))
            {
                menuUser(socket);
            }
            break;
        case 3:
            send(socket, "SEARCH_BY_PAROLACHIAVE", strlen("SEARCH_BY_PAROLACHIAVE"), 0);
            break;
        case 4:
            send(socket, "SEARCH_BY_ISBN", strlen("SEARCH_BY_ISBN"), 0);
            break;
        case 5:
            return;
        default:
            printf("Opzione non valida! Riprova.\n");
        }
    }
}

void menuUser(int socket)
{
    int choice;
    while (1)
    {
        // pulisci lo schermo con? clrscr(); system(clear); system(crl); printf("\033[2J"); boh
        printf("\n--- Menu ---\n");
        printf("1. Ricerca di un libro tramite parola chiave.\n");
        printf("2. Ricerca di un libro tramite ISBN.\n");
        printf("3. Aggiungi al carrello un libro tramite ISBN\n");
        printf("4. Checkout\n");
        printf("5. Logout\n");
        printf("6.  Exit.\n");
        printf("Inserisci qui la tua opzione: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            send(socket, "SEARCH_BY_PAROLACHIAVE", strlen("SEARCH_BY_PAROLACHIAVE"), 0);
            break;
        case 2:
            send(socket, "SEARCH_BY_ISBN", strlen("SEARCH_BY_ISBN"), 0);
            break;
        case 3:
            send(socket, "ADD_TO_CART", strlen("ADD_TO_CART"), 0);
            break;
        case 4:
            send(socket, "CHECKOUT", strlen("CHECKOUT"), 0);
            break;
        case 5:
            logout();
            menuGuest(socket);
            break;
        case 6:
            return;
        default:
            printf("Opzione non valida! Riprova.\n\n");
        }
    }
}