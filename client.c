#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "client.h"
#include "utente.h"
#include "define.h"
#include <fcntl.h>

int client_connesso, ISBN;
int *puntatoreInt;
char *user_name, *user_email, *charRisposta;
char *parolaChiave, *email, *password, *nome, *charPtISBN, *buffer, *bufferDeluxe, *rispostaValidaPointer = "RISPOSTA_VALIDA";

int main()
{
    client_connesso = NON_CONNESSO;

    user_name = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    user_email = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    parolaChiave = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    email = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    password = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    nome = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    charPtISBN = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    buffer = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    charRisposta = (char *)malloc(MAX_MESSAGE_LENGTH);
    bufferDeluxe = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char) * 10);

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

    int flags = fcntl(socket_desc, F_GETFL, 0);

    printf("Connessione al server avvenuta con successo!♥\n");
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
            funzioneSearchParolaChiave(socket);
            break;
        case 4:
            funzioneSearchISBN(socket);
            break;
        case 5:
            exit(0);
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
            funzioneSearchParolaChiave(socket);
            break;
        case 2:
            funzioneSearchISBN(socket);
            break;
        case 3:
            funzioneAddToCart(socket);
            break;
        case 4:
            funzioneCheckout(socket);
            break;
        case 5:
            logout();
            menuGuest(socket);
            break;
        case 6:
            exit(0);
        default:
            printf("Opzione non valida! Riprova.\n\n");
        }
    }
}

void menuAdmin(int socket)
{
    int choice;
    while (1)
    {
        printf("\n--- Menu ---\n");
        printf("1. Elenco di tutti i libri.\n");
        printf("2. Elenco di tutti i prestiti.\n");
        printf("3. Modifica valore K.\n");
        printf("4. Ricerca Libro per Prola Chiave.\n");
        // printf("5. Ricerca Libro per Titolo.\n");
        // printf("6. Ricerca Libro per ISBN.\n");
        // printf("7. Exit\n");
        printf("5. Exit\n");
        printf("Inserisci qui la tua opzione: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            funzioneElencoLibri(socket);
            break;
        case 2:
            funzioneElencoPrestiti(socket);
            break;
        case 3:
            funzioneModificaK(socket);
            break;
        case 4:
            funzioneSearchParolaChiave(socket);
            break;
        case 5:
            logout();
            menuGuest(socket);
            break;
        default:
            printf("Opzione non valida! Riprova.\n\n");
        }
    }
}

void funzioneLogin(int socket)
{

    // Mando opzione LOGIN
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "LOGIN\n");
    send(socket, buffer, strlen(buffer), 0);

    // Chiedo i dati
    printf("\nInserisci email: ");
    scanf("%s", email);

    printf("\nInserisci psw: ");
    scanf("%s", password);

    // concateno i dati in un unico buffer
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, email);
    strcat(buffer, ";");
    strcat(buffer, password);

    send(socket, buffer, strlen(buffer), 0);

    // In base alla risposta restituisco un messaggio diverso
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);

    if (strcmp(buffer, "RISPOSTA_VALIDA") == 0)
    {
        printf("Login riuscito correttamente!\n\n");
        menuUser(socket);
    }

    else if (strcmp(buffer, "ADMIN") == 0)
    {
        printf("\nBentornato Libraio!\n\n");
        menuAdmin(socket);
    }
    else
    {
        printf("Login non riuscito.\n\n");
    }
}

void funzioneRegister(int socket)
{

    // Mando comando "REGISTER"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "REGISTER\n");
    send(socket, buffer, strlen(buffer), 0);

    // Prendo i dati che mi servono
    printf("\nInserisci il nome(parola unica): ");
    scanf("%s", nome);

    printf("\nInserisci email: ");
    scanf("%s", email);

    printf("\nInserisci psw: ");
    scanf("%s", password);

    // concateno i dati in un unico buffer
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, nome);
    strcat(buffer, ";");
    strcat(buffer, email);
    strcat(buffer, ";");
    strcat(buffer, password);

    send(socket, buffer, strlen(buffer), 0);

    // Attendi la risposta finale del server (registrazione riuscita o fallita)
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer); // Mostra "Registrazione riuscita" o "Registrazione fallita"
}

void funzioneSearchParolaChiave(int socket)
{

    // Mando comando "SEARCH_BY_PAROLACHIAVE\n"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "SEARCH_BY_PAROLACHIAVE\n");
    send(socket, buffer, strlen(buffer), 0);

    bzero(buffer, MAX_MESSAGE_LENGTH);
    printf("\nInserisci la parola chiave: ");
    scanf("%s", buffer);

    send(socket, buffer, strlen(buffer), 0);

    // Attendi la risposta del server con la lista dei libri;
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH * sizeof(char), 0);
    printf("Risultati della ricerca:\n%s\n", buffer);
}

void funzioneSearchISBN(int socket)
{

    // Mando comando "SEARCH_BY_ISBN\n"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "SEARCH_BY_ISBN\n");
    send(socket, buffer, strlen(buffer), 0);

    bzero(buffer, MAX_MESSAGE_LENGTH);
    printf("\nInserisci l'ISBN: ");
    scanf("%s", buffer);

    // printf("Client:\n %s",buffer);

    send(socket, buffer, strlen(buffer), 0);

    // Attendi la risposta del server con la lista dei libri;
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH * sizeof(char), 0);
    printf("Risultati della ricerca:\n%s\n", buffer);
}

void funzioneSearchCategoria(int socket)
{

    // Mando comando "SEARCH_BY_CATEGORIA\n"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "SEARCH_BY_CATEGORIA\n");
    send(socket, buffer, strlen(buffer), 0);

    bzero(buffer, MAX_MESSAGE_LENGTH);
    printf("\nInserisci la Categoria: ");
    scanf("%s", buffer);

    // printf("Client:\n %s",buffer);

    send(socket, buffer, strlen(buffer), 0);

    // riciclo il buffer per la lista di libri della categoria X
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH * sizeof(char), 0);
    printf("\nRisultati della ricerca:\n%s\n", buffer);
}

void funzioneAddToCart(int socket)
{

    // Mando comando "ADD_TO_CART\n"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "ADD_TO_CART\n");
    send(socket, buffer, strlen(buffer), 0);

    // Mando ISBN
    bzero(buffer, MAX_MESSAGE_LENGTH);
    printf("\nInserisci l'ISBN da aggiungere al carrello: ");
    scanf("%s", buffer);
    send(socket, buffer, strlen(buffer), 0);

    // Attendi la risposta finale del server
    bzero(buffer, MAX_MESSAGE_LENGTH); // Pulisci il buffer
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}

void funzioneCheckout(int socket)
{

    // Mando comando "CHECKOUT\n"
    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "CHECKOUT\n");
    send(socket, buffer, strlen(buffer), 0);

    send(socket, email, strlen(buffer), 0);

    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}

void funzioneElencoLibri(int socket)
{

    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "ELENCO_LIBRI\n");
    send(socket, buffer, strlen(buffer), 0);

    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("Ecco l'elenco completo di tutti i libri:\n");
    printf("%s\n", buffer);
}

void funzioneElencoPrestiti(int socket)
{

    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "ELENCO_PRESTITI\n");
    send(socket, buffer, strlen(buffer), 0);

    bzero(bufferDeluxe, MAX_MESSAGE_LENGTH);
    recv(socket, bufferDeluxe, MAX_MESSAGE_LENGTH * sizeof(char) * 10, 0);
    printf("Ecco l'elenco completo di tutti i prestiti:\n");
    printf("%s\n", bufferDeluxe);
}

void funzioneModificaK(int socket)
{

    bzero(buffer, MAX_MESSAGE_LENGTH);
    strcpy(buffer, "MODIFICA_K\n");
    send(socket, buffer, strlen(buffer), 0);

    // Riceve K attuale
    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);

    printf("\nIl valore attuale di K è: %s", buffer);
    printf("\nvuoi cambiare valore? (s/n): ");
    scanf("%s", charRisposta);

    bzero(buffer, MAX_MESSAGE_LENGTH);

    if (strcmp(charRisposta, "s") == 0)
    {

        printf("\nInserisci il nuovo valore: ");
        scanf("%s", buffer);
        send(socket, buffer, strlen(buffer), 0);
    }
    else
    {

        strcpy(buffer, "-1");
        send(socket, buffer, strlen(buffer), 0);
    }

    bzero(buffer, MAX_MESSAGE_LENGTH);
    recv(socket, buffer, MAX_MESSAGE_LENGTH, 0);
    printf("%s\n", buffer);
}