#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utente.h"

void registerUser(int socket, char name[100], char email[100], char password[30]) {
    FILE *file = fopen("utentiRegistrati.txt", "a");
    if (!file) {
        perror("Errore durante l'apertura del file utenteRegistrati.txt in utente.registerUser\n");
        return;
    }

    recv(socket, name, 100, 0);
    recv(socket, email, 100, 0);
    recv(socket, password, 30, 0);


    fprintf(file, "%s | %s | %s | 0 \n", name, email, password); // 0 libri presi in prestito all'inizio
    fclose(file);

    send(socket, "Utente registrato correttamente!", strlen("Utente registrato correttamente!"), 0);
}

int loginUser(int socket, char *EMAIL , char *PSW) {
    char email[100], password[30], line[256];

    FILE *file = fopen("utentiRegistrati.txt", "r");
    if (!file) {
        perror("Impossibile aprire file 'utentiRegistrati.txt' in utente.loginUser\n");
        return 0;
    }

    recv(socket, email, 100, 0);
    recv(socket, password, 30, 0);
    strcpy(EMAIL, email);
    strcpy(PSW, password);

    while (fgets(line, sizeof(line), file) != NULL) {
        char *tempMail, *tempPsw, *nomeUtente;

        char *token = strtok(line, "|");
        nomeUtente = token;

        token = strtok(NULL, "|");
        tempMail = token;

        token = strtok(NULL, "|");
        tempPsw = token;

        if (strcmp(tempMail, EMAIL) == 0 && strcmp(tempPsw, PSW) == 0) {
            
            //DA IMPLEMENTARE: LOGIN UTENTE!!!//

            fclose(file);
            printf("Login riuscito per l'utente: %s\n", nomeUtente);
            return 1; // Login riuscito
            break;
        }
            
    }

    //In caso non trovo nessun match
    send(socket, "Errore durante il login", strlen("Errore durante il login"), 0);
    fclose(file);
    return 0;
}
