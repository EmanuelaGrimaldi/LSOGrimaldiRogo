#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "utente.h"  // Includiamo utente.h
#include "libro.h"   // Includiamo libro.h
#include "carrello.h" // Includiamo carrello.h

#define MAX 1024

void handleClient(int socket);

int main() {
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    // Creazione socket server
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // Bind
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        return 1;
    }

    // Ascolto
    listen(server_sock, 3);
    printf("Server listening on port 8080...\n");

    c = sizeof(struct sockaddr_in);
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t *)&c))) {
        printf("Client connected\n");
        handleClient(client_sock);
        close(client_sock);
    }

    if (client_sock < 0) {
        printf("Accept failed\n");
        return 1;
    }

    close(server_sock);
    return 0;
}

void handleClient(int socket) {
    char client_message[MAX];
    char userID[100];
    int read_size;

    // Ricevi comandi dal client
    while ((read_size = recv(socket, client_message, MAX, 0)) > 0) {
        client_message[read_size] = '\0';

        if (strcmp(client_message, "REGISTER") == 0) {
            char name[100], email[100];
            recv(socket, name, 100, 0);
            recv(socket, email, 100, 0);
            registerUser(socket, name, email);

        } else if (strcmp(client_message, "LOGIN") == 0) {
            recv(socket, userID, 100, 0);
            loginUser(socket, userID);

        } else if (strcmp(client_message, "SEARCH") == 0) {
            searchBook(socket);

        } else if (strcmp(client_message, "ADD_TO_CART") == 0) {
            char bookTitle[100];
            recv(socket, bookTitle, 100, 0);
            addBookToCart(socket, userID); // Utilizza l'ID dell'utente loggato

        } else if (strcmp(client_message, "CHECKOUT") == 0) {
            checkout(socket, userID); // Utilizza l'ID dell'utente loggato

        } else {
            send(socket, "Invalid command", strlen("Invalid command"), 0);
        }
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("Recv failed");
    }
}

