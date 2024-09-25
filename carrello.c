#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"

// Aggiunge un libro al carrello dell'utente
void addBookToCart(int socket, const char *userID)
{
    char bookTitle[100];
    FILE *cartFile = fopen("carrello.txt", "a");

    if (!cartFile)
    {
        perror("Could not open cart file");
        send(socket, "Errore durante l'apertura del file carrello.txt in carrello.c\n", strlen("Errore durante l'apertura del file carrello.txt in carrello.c\n"), 0);
        return;
    }

    // Ricevi il titolo del libro dal client
    recv(socket, bookTitle, 100, 0);

    // Verifica se il libro è disponibile
    if (isBookAvailable(bookTitle))
    {
        fprintf(cartFile, "%s|%s\n", userID, bookTitle); // Scrivi l'ID utente e il titolo del libro nel carrello
        send(socket, "Libro aggiunto al carrello correttamente", strlen("Libro aggiunto al carrello correttamente"), 0);
    }
    else
    {
        send(socket, "Libro non disponibile", strlen("Libro non disponibile"), 0);
    }

    fclose(cartFile);
}

// Processo di checkout: rimuove i libri dal carrello e li marca come presi in prestito
void checkout(int socket, const char *userID)
{
    char line[256];
    char tempFile[] = "temp.txt";
    FILE *cartFile = fopen("carrello.txt", "r");
    FILE *temp = fopen(tempFile, "w");

    if (!cartFile || !temp)
    {
        perror("Could not open files for checkout");
        send(socket, "Errore durante il checkout", strlen("Errore durante il checkout"), 0);
        return;
    }

    int booksCheckedOut = 0;

    while (fgets(line, sizeof(line), cartFile))
    {
        char *cartUserID = strtok(line, "|");
        char *bookTitle = strtok(NULL, "|");

        if (strcmp(cartUserID, userID) == 0)
        {
            // Marca il libro come preso in prestito dall'utente
            borrowBook(userID, bookTitle);
            booksCheckedOut++;
        }
        else
        {
            // Se non è il libro dell'utente corrente, scrivilo nel file temporaneo
            fprintf(temp, "%s|%s", cartUserID, bookTitle);
        }
    }

    fclose(cartFile);
    fclose(temp);

    // Rimuovi il file del carrello originale e rinominalo con il nuovo file  ---- non ho capito
    remove("carrello.txt");
    rename(tempFile, "carrello.txt");

    if (booksCheckedOut > 0)
    {
        send(socket, "Checkout completato", strlen("Checkout completato"), 0);
    }
    else
    {
        send(socket, "Non vi sono libri nel carrello!", strlen("Non vi sono libri nel carrello!"), 0);
    }
}

// Funzione di supporto che verifica se un libro è disponibile
int isBookAvailable(const char *bookTitle)
{
    FILE *libraryFile = fopen("libreria.json", "r");
    struct json_object *parsed_json;
    struct json_object *libri;
    struct json_object *libro;
    size_t n_libri;

    int risposta = RISPOSTA_INVALIDA;

    if (!libraryFile)
    {
        perror("Errore durante l'apertura del file libreria.json");
        return 0;
    }

    char buffer[1024];
    fread(buffer, 1024, 1, libraryFile);
    fclose(libraryFile);

    parsed_json = json_tokener_parse(buffer);
    json_object_object_get_ex(parsed_json, "libri", &libri);
    n_libri = json_object_array_length(libri);

    for (size_t i = 0; i < n_libri; i++)
    {
        libro = json_object_array_get_idx(libri, i);
        const char *titolo = json_object_get_string(json_object_object_get(libro, "titolo"));

        if (strcmp(bookTitle, titolo) == 0)
        {
            int totCopieDisponibili = json_object_get_int(json_object_object_get(libro, "totCopieDisponibili"));
            return totCopieDisponibili > 0;
            if (totCopieDisponibili > 0)
            {
                risposta = RISPOSTA_VALIDA;
                break;
            }
        }
    }

    return risposta;
}

void borrowBook(const char *userID, char *bookTitle)
{
    // Apriamo il file della libreria per aggiornare le informazioni sui libri
    FILE *libroFile = fopen("libreria.JSON", "r+");
    if (libroFile == NULL)
    {
        printf("Errore durante l'apertura del file libreria.json.\n");
        return;
    }

    // Leggiamo il contenuto del file JSON
    char line[256];
    char jsonBuffer[1024] = "";
    int found = 0;

    while (fgets(line, sizeof(line), libroFile))
    {
        // Verifica se il libro è presente
        if (strstr(line, bookTitle) != NULL)
        {
            found = 1;

            // Qui dovresti analizzare il JSON e aggiornare i campi appropriati
            // (totCopie, totCopieDisponibili, totCopiePrestate)
            // Assumiamo che tu abbia una funzione che gestisce questa logica
            // Per esempio, potresti usare una libreria per la manipolazione del JSON

            // Simuliamo l'aggiornamento delle copie
            // (Dovresti implementare la logica per modificare effettivamente il JSON)
            printf("Borrowing book: %s for user: %s\n", bookTitle, userID);
        }
        else
        {
            strcat(jsonBuffer, line); // Mantieni le righe esistenti
        }
    }

    if (!found)
    {
        printf("Book not found in the library.\n");
    }
    else
    {
        // Scrivi le modifiche nel file della libreria
        fseek(libroFile, 0, SEEK_SET);        // Torna all'inizio del file
        fprintf(libroFile, "%s", jsonBuffer); // Scrivi il buffer aggiornato
    }

    fclose(libroFile);

    // Aggiungi il libro al carrello dell'utente
    FILE *carrelloFile = fopen("carrello.txt", "a");
    if (carrelloFile == NULL)
    {
        printf("Errore durante l'apertura del file carrello.txt\n");
        return;
    }

    // Scriviamo nel file del carrello
    fprintf(carrelloFile, "%s | %s\n", userID, bookTitle);
    fclose(carrelloFile);

    printf("Il libro '%s' è stato preso in prestito dall'utente '%s'.\n", bookTitle, userID);
}
