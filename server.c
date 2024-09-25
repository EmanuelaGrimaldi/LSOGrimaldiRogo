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
    char name[MAX_NAME_LENGTH], email[MAX_EMAIL_LENGTH], password[MAX_PWD_LENGTH];
    int read_size;

    // Ricevi comandi dal client
    while ((read_size = recv(socket, client_message, MAX_MESSAGE_LENGTH, 0)) > 0)
    {
        client_message[read_size] = '\0';

        if (strcmp(client_message, "REGISTER") == 0)
        {
            recv(socket, name, 100, 0);
            recv(socket, email, 100, 0);
            recv(socket, password, 30, 0);
            if (callControlliUser(name, email, password) == 1)
            {
                // eisite già
                // rispondi con non_ok e torna indeitro
            }
            else
            {
                // rispondi con tt ok e torna indietro
            }

            // registraUser(name, email, password);  // scrivere su file e inviare al client hce ora è connesso
            //                                       // int registaUser(name, email, password); signatur giusta?
        }
        else if (strcmp(client_message, "LOGIN") == 0)
        {
            // in una f di utente
            recv(socket, email, MAX_NAME_LENGTH, 0);
            recv(socket, password, MAX_PWD_LENGTH, 0);
            if (checkUser(email, password) == 1)
            {

                // come retrival i dati dell'utente? // send(socket, name, strlen(name), 0);
                accedi(email);
            }
            else
                disAccedi();

            // loginUser(socket, name, password); // non c'è
            //
        }
        else if (strcmp(client_message, "SEARCH") == 0)
        {
            searchBook(socket);
        }
        // ---- ATTENZIONE: ADD TO CART ---------------------------------------------------------------------------------------------
        else if (strcmp(client_message, "ADD_TO_CART") == 0)
        {
            // Controllo user/client è collegato. -------

            char bookTitle[100];
            recv(socket, bookTitle, 100, 0);
            addBookToCart(socket, name); // Utilizza l'ID dell'utente loggato------ id= nome o email?
        }
        else if (strcmp(client_message, "CHECKOUT") == 0)
        {
            checkout(socket, name); // Utilizza l'ID dell'utente loggato - name o email?
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
int checkUser(char *email, char *password)
{
    int risposta = RISPOSTA_VALIDA;

    if (risposta && !emailValida())
        risposta = RISPOSTA_INVALIDA;

    if (rispista && !pwdValida())
        risposta = RISPOSTA_INVALIDA;

    return risposta;
}

int callControlliUser(char *name, char *email, char *password)
{
    // callControlliUser(name, email, password);
    //      // boolean/int existUser(email,pwd);
    //      // booelan nomeValido(nome);
    //      // booelan emailValida(email);
    //      // booelan pwdValida(pwd);

    int risposta = RISPOSTA_VALIDA;

    if (risposta && !existUser())
        risposta = RISPOSTA_INVALIDA;

    if (risposta && !nomeValido())
        risposta = RISPOSTA_INVALIDA;

    if (risposta && !emailValida())
        risposta = RISPOSTA_INVALIDA;

    if (rispista && !pwdValida())
        risposta = RISPOSTA_INVALIDA;

    return risposta;
}
