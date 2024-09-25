#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utente.h"

char user_name[MAX_NAME_LENGTH];
char user_email[MAX_EMAIL_LENGTH];
int client_connesso;

void registerUser(int socket, char name[100], char email[100], char password[30])
{
    FILE *file = fopen("utentiRegistrati.txt", "a");
    if (!file)
    {
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

int loginUser(int socket, char *EMAIL, char *PSW)
{
    char email[MAX_EMAIL_LENGTH], password[MAX_PWD_LENGTH], line[256];

    FILE *file = fopen("utentiRegistrati.txt", "r");
    if (!file)
    {
        perror("Impossibile aprire file 'utentiRegistrati.txt' in utente.loginUser\n");
        return 0;
    }

    recv(socket, email, MAX_EMAIL_LENGTH, 0);
    recv(socket, password, MAX_PWD_LENGTH, 0);
    strcpy(EMAIL, email);
    strcpy(PSW, password);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int risposta;

        char *tempMail, *tempPsw, *nomeUtente;

        char *token = strtok(line, "|");
        nomeUtente = token;

        token = strtok(NULL, "|");
        tempMail = token;

        token = strtok(NULL, "|");
        tempPsw = token;

        if (strcmp(tempMail, EMAIL) == 0 && strcmp(tempPsw, PSW) == 0)
        {

            // DA IMPLEMENTARE: LOGIN UTENTE!!!//
            accedi(EMAIL);

            fclose(file);
            printf("Login riuscito per l'utente: %s\n", nomeUtente);
            risposta = RISPOSTA_VALIDA;
        }
        else
        {
            fclose(file);
            printf("Login NON riuscito per l'utente: %s\n", nomeUtente);
            risposta = RISPOSTA_INVALIDA;
        }

        return risposta;
    }

    // In caso non trovo nessun match
    send(socket, "Errore durante il login", strlen("Errore durante il login"), 0);
    fclose(file);
    return 0;
}

void setEmailUser(char *email)
{
    strcpy(user_email, email);
}

void accedi(char *email)
{
    client_connesso = CONNESSO;
    strcpy(user_name, "");
    strcpy(user_email, email);
}

void disAccedi()
{
    client_connesso = NON_CONNESSO;
    strcpy(user_name, "");
    strcpy(user_email, "");
}

int existUser(char *user)
{
    // da implementare
    return RISPOSTA_VALIDA;
}

int emailValida(char *email)
{
    // da implementare
    return RISPOSTA_VALIDA;
}

int pwdValida(char *pwd)
{
    // da implementare
    return RISPOSTA_VALIDA;
}
