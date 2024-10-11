#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#include "utente.h"
#include "define.h"

int client_connesso, ISBN;
char *user_name, *user_email;
char *parolaChiave, *email, *password, *nome, *charPtISBN, *buffer, *rispostaValidaPointer = "RISPOSTA_VALIDA";
int ISBN;

int main()
{
    client_connesso = NON_CONNESSO;

    user_name = (char*)malloc(25*sizeof(char));
    user_email = (char*)malloc(25*sizeof(char));
    parolaChiave = (char*)malloc(15*sizeof(char));
    email = (char*)malloc(25*sizeof(char));
    password = (char*)malloc(15*sizeof(char));
    nome = (char*)malloc(25*sizeof(char));
    charPtISBN = (char*)malloc(10*sizeof(char));
    buffer = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));

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
            funzioneRegister(socket);
            break;
        case 2:
            funzioneLogin(socket);
            break;
        case 3:
            funzioneSearchParolaChiave (socket);
            break;
        case 4:
            funzioneSearchISBN(socket);
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
            funzioneSearchParolaChiave (socket);
            break;
        case 2:
            funzioneSearchISBN(socket);
            break;
        case 3:
            funzioneAddToCart(socket);
            break;
        case 4:
            send(socket, "CHECKOUT", strlen("CHECKOUT"), 0);
            send(socket, email, strlen(email), 0);
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


void funzioneLogin(int socket){

    send(socket, "LOGIN\0", strlen("LOGIN\0"), 0);

    // Attendi la richiesta del server per l'email
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);  // Mostra "Inserisci la tua email: "

    // Inserisci l'email e inviala al server
    fgets(email, sizeof(email), stdin);
    send(socket, email, strlen(email), 0);

    // Attendi la richiesta del server per la password
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);  // Mostra "Inserisci la tua password: "

    // Inserisci la password e inviala al server
    fgets(password, sizeof(password), stdin);
    send(socket, password, strlen(password), 0);

    // Comparo se la stringa ricevuta è "RISPOSTA_VALIDA" , come int dava problemi
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    if ( strcmp (recv(socket, buffer, MAX_MESSAGE_LENGTH, 0) , rispostaValidaPointer) == 0)                            //*********************************** 
    {   
        printf("Login riuscito correttamente!\n\n");
        menuUser(socket);
    }
    else{
        printf("Login non riuscito.\n\n");
    }
}

void funzioneRegister(int socket){

    send(socket, "REGISTER\0", strlen("REGISTER\0"), 0);

    // Attendi la richiesta del server per il nome completo
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);  // Mostra "Inserisci il tuo nome completo: "

    // Inserisci il nome completo e invialo al server
    //fgets(nome, sizeof(nome), stdin);
    scanf("%s", nome);
    send(socket, nome, strlen(nome), 0);

    // Attendi la richiesta del server per l'email
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);  // Mostra "Inserisci la tua email: "

    // Inserisci l'email e inviala al server
    //fgets(email, sizeof(email), stdin);
    scanf("%s", email);
    send(socket, email, strlen(email), 0);

    // Attendi la richiesta del server per la password
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);  // Mostra "Inserisci la tua password: "

    // Inserisci la password e inviala al server
    //fgets(password, sizeof(password), stdin);
    scanf("%s", password);
    send(socket, password, strlen(password), 0);

    // Attendi la risposta finale del server (registrazione riuscita o fallita)
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);  // Mostra "Registrazione riuscita" o "Registrazione fallita"

}

void funzioneSearchParolaChiave (int socket){

    send(socket, "SEARCH_BY_PAROLACHIAVE\0", strlen("SEARCH_BY_PAROLACHIAVE\0"), 0);
    
    // Attendi la richiesta del server per la parola chiave
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);

    // Mando parola chiave
    fgets(parolaChiave, sizeof(parolaChiave), stdin);
    send(socket, parolaChiave, strlen(parolaChiave), 0);

    // Attendi la risposta del server con la lista dei libri;
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);

    // Mostra i risultati al client
    printf("Risultati della ricerca:\n%s\n", buffer);

    // Attendi la risposta finale del server
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}

void funzioneSearchISBN (int socket){

    send(socket, "SEARCH_BY_ISBN\0", strlen("SEARCH_BY_ISBN\0"), 0);

    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);

    fgets(charPtISBN, sizeof(charPtISBN), stdin);                
    send(socket, charPtISBN, strlen(charPtISBN), 0);        

    // Attendi la risposta del server con la lista dei libri;
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);

    // Mostra i risultati al client
    printf("Risultati della ricerca:\n%s\n", buffer);

    // Attendi la risposta finale del server
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}

void funzioneAddToCart(int socket){

    send(socket, "ADD_TO_CART\0", strlen("ADD_TO_CART\0"), 0);
    
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s", buffer);

    fgets(charPtISBN, sizeof(charPtISBN), stdin);          
    send(socket, charPtISBN, strlen(charPtISBN), 0);   

    // Attendi la risposta finale del server
    memset(buffer, 0, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}