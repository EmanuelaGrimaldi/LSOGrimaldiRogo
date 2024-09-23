#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX 1024

void menu(int socket);
void login(int socket);
void registerUser(int socket);
void searchBook(int socket);
void addToCart(int socket);
void checkout(int socket);

int main() {
    int socket_desc;
    struct sockaddr_in server;
    char message[MAX], server_reply[MAX];

    // Creazione socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    // Connessione al server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Connected to server!\n");
    menu(socket_desc);
    close(socket_desc);
    return 0;
}

void menu(int socket) {
    int choice;
    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Search Book\n");
        printf("4. Add to Cart\n");
        printf("5. Checkout\n");
        printf("6. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
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
                addToCart(socket);
                break;
            case 5:
                checkout(socket);
                break;
            case 6:
                return;
            default:
                printf("Invalid option. Try again.\n");
        }
    }
}

void registerUser(int socket) {
    char name[100], email[100];
    char server_reply[MAX];

    printf("Enter your name: ");
    scanf("%s", name);
    printf("Enter your email: ");
    scanf("%s", email);

    // Invia dati di registrazione al server
    send(socket, "REGISTER", strlen("REGISTER"), 0);
    send(socket, name, strlen(name), 0);
    send(socket, email, strlen(email), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX, 0);
    printf("Server: %s\n", server_reply);
}

void login(int socket) {
    char email[100];
    char server_reply[MAX];

    printf("Enter your email: ");
    scanf("%s", email);

    // Invia dati di login al server
    send(socket, "LOGIN", strlen("LOGIN"), 0);
    send(socket, email, strlen(email), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX, 0);
    printf("Server: %s\n", server_reply);
}

void searchBook(int socket) {
    char bookTitle[100];
    char server_reply[MAX];

    printf("Enter book title to search: ");
    scanf("%s", bookTitle);

    // Invia richiesta di ricerca al server
    send(socket, "SEARCH", strlen("SEARCH"), 0);
    send(socket, bookTitle, strlen(bookTitle), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX, 0);
    printf("Server: %s\n", server_reply);
}

void addToCart(int socket) {
    char bookTitle[100];
    char server_reply[MAX];

    printf("Enter book title to add to cart: ");
    scanf("%s", bookTitle);

    // Invia richiesta di aggiungere al carrello
    send(socket, "ADD_TO_CART", strlen("ADD_TO_CART"), 0);
    send(socket, bookTitle, strlen(bookTitle), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX, 0);
    printf("Server: %s\n", server_reply);
}

void checkout(int socket) {
    char server_reply[MAX];

    // Invia richiesta di checkout al server
    send(socket, "CHECKOUT", strlen("CHECKOUT"), 0);

    // Ricevi risposta dal server
    recv(socket, server_reply, MAX, 0);
    printf("Server: %s\n", server_reply);
}
