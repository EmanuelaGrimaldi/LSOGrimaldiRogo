#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"
#include "cJSON.h"

//---------------------------------------------------------------------------------------------------------------------------------- da rivedere
void addBookToCart(int socket, const char *email, char * bookTitle)
{
    FILE *cartFile = fopen("carrelloUtenti.txt", "a");

    if (!cartFile)
    {
        perror("Could not open cart file");
        send(socket, "Errore durante l'apertura del file carrello.txt in carrello.c\n", strlen("Errore durante l'apertura del file carrello.txt in carrello.c\n"), 0);
        return;
    }

    if (isBookAvailable(bookTitle))
    {
        char buffer[1024];
        int trovato = 0;
        char emailInput[] = email;

        // Leggiamo il file riga per riga
        while (fgets(buffer, sizeof(buffer), cartFile)) {
            char tempEmailStringa[100];
            char *tempEmail = strtok(buffer, "|");
            char nuovaRigaCarrello[1024];

            // Controlliamo se il nome esiste già
            if (tempEmail != NULL) {
                strncpy(tempEmailStringa, tempEmail, 100);

                if (strcmp(tempEmailStringa, emailInput) == 0) 
                {
                    trovato = 1;
                    // Se l'utente è trovato, aggiungiamo il prodotto
                    snprintf(nuovaRigaCarrello, sizeof(nuovaRigaCarrello), "%s|", bookTitle);

                    // Copiamo tutti i prodotti già esistenti
                    tempEmail = strtok(NULL, "\n");
                    if (tempEmail != NULL) {
                        strncat(nuovaRigaCarrello, tempEmail, sizeof(nuovaRigaCarrello) - strlen(nuovaRigaCarrello) - 1);
                    }

                    // Aggiungiamo il nuovo prodotto
                    //strncat(nuovaRigaCarrello, "|", sizeof(nuovaRigaCarrello) - strlen(nuovaRigaCarrello) - 1);
                    strncat(nuovaRigaCarrello, bookTitle, sizeof(nuovaRigaCarrello) - strlen(nuovaRigaCarrello) - 1);

                    // Torniamo indietro di una riga per riscrivere quella aggiornata
                    fseek(cartFile, -strlen(buffer), SEEK_CUR);
                    fprintf(cartFile, "%s\n", nuovaRigaCarrello);
                    break;
                }
            }
        }
        if (trovato == 0) {
            fseek(cartFile, 0, SEEK_END);
            fprintf(cartFile, "%s|%s|\n", email, bookTitle);
        }

        fclose(cartFile);      
    }
    else
    {
        send(socket, "Libro non disponibile", strlen("Libro non disponibile"), 0);
    }

    fclose(cartFile);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------INCOMPLETO!
void checkout(int socket, const char *email)
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
        char *tempEmail = strtok(NULL, "|");

        if (strcmp(email, tempEmail) == 0)
        {
            /*
            ----------------------------------------------------------------------------------------------------------------------------------------------TODO: 
            Prendi in prestito tutti i libri che vengono dopo la mia email
            Per ogni libro: aggiornaLibreriaJson();
            cancellare rigo attuale di carrello.txt
            */
        
        }
    }

    fclose(cartFile);
    fclose(temp);
}

//---------------------------------------------------------------------------------------------------------------------------------------- Da verificare
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
            //return totCopieDisponibili > 0;                           ------------------------------------------------------???????
            if (totCopieDisponibili > 0)
            {
                risposta = RISPOSTA_VALIDA;
                break;
            }
        }
    }

    return risposta;
}


// Funzione per aggiornare le copie disponibili di un libro con un dato ISBN
int aggiornaLibreriaJson(const char *ISBN, int numeroCopia) 
{
    char *json_tempLibreria = copiaLibreriaJson();
    if (json_tempLibreria == NULL) {
        printf("Errore nel leggere il file.\n");
        return 0;
    }

    // Parsing del file JSON
    cJSON *json = cJSON_Parse(json_tempLibreria);
    if (json == NULL) {
        fprintf(stderr, "Errore nel parsing del JSON\n");
        free(json_tempLibreria);
        return 0;
    }

    // Otteniamo l'array "libri"
    cJSON *libri = cJSON_GetObjectItem(json, "libri");
    if (!cJSON_IsArray(libri)) {
        fprintf(stderr, "Formato JSON non valido\n");
        cJSON_Delete(json);
        free(json_tempLibreria);
        return 0;
    }

    // Iteriamo sugli elementi dell'array "libri"
    cJSON *libro;
    cJSON_ArrayForEach(libro, libri) {
        cJSON *isbn = cJSON_GetObjectItem(libro, "ISBN");
        cJSON *copie_disponibili = cJSON_GetObjectItem(libro, "copie_disponibili");

        if (cJSON_IsString(isbn) && (isbn->valuestring != NULL) &&
            cJSON_IsNumber(copie_disponibili)) {
            // Controlliamo se l'ISBN corrisponde a quello cercato
            if (strcmp(isbn->valuestring, isbn_da_cercare) == 0) {
                // Incrementiamo il numero di copie disponibili di 1
                copie_disponibili->valueint += 1;
                printf("Copie disponibili aggiornate a: %d\n", copie_disponibili->valueint);

                // Scriviamo il file JSON aggiornato
                char *json_string = cJSON_Print(json); // Convertiamo l'oggetto JSON in stringa
                if (scrivi_file(filename, json_string)) {
                    printf("Il file JSON è stato aggiornato con successo.\n");
                } else {
                    printf("Errore nella scrittura del file JSON.\n");
                }

                // Pulizia della memoria
                cJSON_Delete(json);
                free(json_tempLibreria);
                free(json_string);
                return 1; // Aggiornamento riuscito
            }
        }
    }

    printf("Il libro con ISBN %s non è stato trovato.\n", isbn_da_cercare);
    cJSON_Delete(json);
    free(json_tempLibreria);
    return 0; // Libro non trovato
}

// Funzione per leggere il file JSON in memoria
char* copiaLibreriaJson(){
    FILE *file = fopen("libreria.json", "r");
    if (file == NULL) {
        perror("Errore nell'aprire il file");
        return NULL;
    }

    //ottengo file lenght
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *tempLibreria = malloc(length + 1);
    if (tempLibreria) {
        fread(tempLibreria, 1, length, file);
    }
    fclose(file);

    tempLibreria[length] = '\0';
    return tempLibreria;
}

// Funzione per scrivere il file JSON aggiornato
int aggiornaLibreriaJson(const char *filename, const char *tempLibreria) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Errore nell'aprire il file in scrittura");
        return 0;
    }
    
    fputs(tempLibreria, file);
    fclose(file);
    return 1;
}