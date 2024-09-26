#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libpq-fe.h>
#include <unistd.h>
#include "client.h"
#include "utente.h"
#include "define.h"

int main()
{
    client_connesso = NON_CONNESSO;
    user_name = NULL;
    user_email = NULL;

    int socket_desc;
    struct sockaddr_in server;
    char message[MAX_MASSAGE_LENGTH], server_reply[MAX_MASSAGE_LENGTH];

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
        printf("3. Ricerca di un libro\n");
        printf("4. Chiudi\n");
        printf("Inserisci qui la tua opzione: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            registerUser(socket);
            break;
        case 2:
            login(socket);
            break;
        case 3:
            searchBook(socket);
            break;
        case 4:
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
        printf("1. Ricerca di un libro\n");
        printf("2. Aggiungi al carrello un libro\n");
        printf("3. Checkout\n");
        printf("4. Esci\n");
        printf("Inserisci qui la tua opzione: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            searchBook(socket);
            break;
        case 2:
            addToCart(socket);
            break;
        case 3:
            checkout(socket);
            break;
        case 4:
            disaccedi();
            menuGuest();
            break;
        default:
            printf("Opzione non valida! Riprova.\n");
        }
    }
}

void registerUser(int socket)
{
    char name[100], email[100], password[30];
    char server_reply[MAX_MASSAGE_LENGTH];

    printf("Inserisci qui il tuo nome per intero: ");
    scanf("%s", name);
    printf("Inserisci qui la tua email: ");
    scanf("%s", email);
    printf("Inserisci qui la tua password: ");
    scanf("%s", password);

    // Invia dati di registrazione al server
    send(socket, "REGISTER", strlen("REGISTER"), 0);
    send(socket, name, strlen(name), 0);
    send(socket, email, strlen(email), 0);
    send(socket, password, strlen(password), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX_MASSAGE_LENGTH, 0);
    printf("Server: %s\n", server_reply);
}

void login(int socket)
{
    char email[100], password[30];
    char server_reply[MAX_MASSAGE_LENGTH];
    char server_reply_email[MAX_MASSAGE_LENGTH];

    printf("Inserisci la tua email: ");
    scanf("%s", email);
    printf("Inserisci qui la tua password: ");
    scanf("%s", password);

    // Invia dati di login al server
    send(socket, "LOGIN", strlen("LOGIN"), 0);
    send(socket, email, strlen(email), 0);
    send(socket, password, strlen(password), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX_MASSAGE_LENGTH, 0);
    printf("Server: %s\n", server_reply);

    // Ridirezionamento
    if (strcmp(server_reply, "ok") == 0)
    {
        menuUser();
    }
    else
        menuGuest();
}

void searchBook(int socket)
{
    char bookTitle[100];
    char server_reply[MAX_MASSAGE_LENGTH];

    printf("Inserisci la parola chiave per la ricerca del libro: ");
    scanf("%s", bookTitle);

    // Invia richiesta di ricerca al server
    send(socket, "SEARCH", strlen("SEARCH"), 0);
    send(socket, bookTitle, strlen(bookTitle), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX_MASSAGE_LENGTH, 0);
    printf("Server: %s\n", server_reply);
}

void addToCart(int socket)
{
    char bookTitle[100];
    char server_reply[MAX_MASSAGE_LENGTH];

    printf("Inserisci il titolo del libro da aggiungere al carrello: ");
    scanf("%s", bookTitle);

    // Invia richiesta di aggiungere al carrello
    send(socket, "ADD_TO_CART", strlen("ADD_TO_CART"), 0);
    send(socket, bookTitle, strlen(bookTitle), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX_MASSAGE_LENGTH, 0);
    printf("Server: %s\n", server_reply);
}

void checkout(int socket)
{
    char server_reply[MAX_MASSAGE_LENGTH];

    // Invia richiesta di checkout al server
    send(socket, "CHECKOUT", strlen("CHECKOUT"), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX_MASSAGE_LENGTH, 0);
    printf("Server: %s\n", server_reply);
}