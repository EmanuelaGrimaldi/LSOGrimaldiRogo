#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utente.h"

void registerUser(int socket, char name[100], char email[100]) {
    FILE *file = fopen("utentiRegistrati.txt", "a");
    if (!file) {
        perror("Could not open file");
        return;
    }

    recv(socket, name, 100, 0);
    recv(socket, email, 100, 0);

    fprintf(file, "%s|%s|0|\n", name, email); // 0 libri presi in prestito all'inizio
    fclose(file);

    send(socket, "User registered successfully", strlen("User registered successfully"), 0);
}

int loginUser(int socket, char *ID) {
    char id[100], line[256];
    FILE *file = fopen("utentiRegistrati.txt", "r");
    if (!file) {
        perror("Could not open file");
        return 0;
    }

    recv(socket, id, 100, 0);
    strcpy(ID, id);

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, "|");
        if (token != NULL && strcmp(token, id) == 0) {
            send(socket, "Login successful", strlen("Login successful"), 0);
            fclose(file);
            return 1;
        }
    }

    send(socket, "Login failed", strlen("Login failed"), 0);
    fclose(file);
    return 0;
}
