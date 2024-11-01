#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"
#include <libpq-fe.h>

char *charISBN, *charNumeroCopie, *charISBN, *bufferCart, *singoloISBN;
char bufferCh[MAX_MESSAGE_LENGTH], toAppend[MAX_MESSAGE_LENGTH];
int numeroCopie, i;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void aggiungiLibroAlCarrello(int socket, char *email, char * ISBN, char *conninfo)
{
     PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    printf("\npre check in carrello.c\n\n"); 

    if (isLibroDisponibile(ISBN, conninfo) == RISPOSTA_INVALIDA){
        printf("Non vi sono copie disponibili per questo libro.\n\n");

    } else {

        printf("\n POST CHECK\n Valori che ricevo: email: %s , ISBN: %s\n", email, ISBN);
        //Creo ed eseguo la query
        const char *paramValues[2] = { ISBN, email };    
        printf("2\n");                                             
        PGresult *res = PQexecParams(conn,
                                    "INSERT INTO carrello (isbnCarrello, emailCarrello) VALUES ($1, $2)",
                                    2,        // Numero di parametri
                                    NULL,     // OID dei parametri (NULL per default)
                                    paramValues, // Valori dei parametri
                                    NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                    NULL,     // Formato dei parametri (NULL per stringhe)
                                    0);       // Formato del risultato (0 = testo)

        printf("3\n");

        // Verifica se l'operazione è andata a buon fine
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Errore durante l'inserimento: %s", PQerrorMessage(conn));
        } else {
            printf("Elemento aggiunto al carrello con successo!\n");
        }
  
        // Libera la memoria allocata per il risultato
        PQclear(res);
        PQfinish(conn);

    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
char* checkout(int socket, char *email, char *conninfo)
{
    free(bufferCh); free(bufferCart);

    bufferCh = (char*)malloc(MAX_MESSAGE_LENGTH);
    bufferCart = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));

    PGconn *conn = PQconnectdb(conninfo);


    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    //STEP 1: Mi prendo tutti i libri messi nel carrello da quell'utente
    const char *paramValues[1] = { email };
    PGresult *res = PQexecParams(conn,
                                "SELECT isbnCarrello FROM carrello WHERE emailCarrello = $1",
                                1,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValues, // Valori dei parametri
                                NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                NULL,     // Formato dei parametri (NULL per stringhe)
                                0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }
    int num_rows = PQntuples(res);

    printf("\n CARRELLO.C - post query\n");

    //Mi salvo nel buffer tutti i libri
    if (num_rows > 0){
        for (i = 0; i < num_rows; i++) {

                if (bufferCh == ""){
                    strcpy(bufferCh, PQgetvalue(res, i, 0));
                    strcat(bufferCh, "\n");
                } else {
                    strcat(bufferCh, PQgetvalue(res, i, 0));
                    strcat(bufferCh, "\n");
                }
        }

        printf("\nDOPO FOR - bufferCh:%s", bufferCh);
    }


    /*STEP 2: per ogni libro:   mi prendo il singolo ISBN
                                controllo ci sia ancora una copia disponibile
                                se c'è, la prendo io + creo un prestito
                                se non c'è, aggiungo l'isbn nell'avviso che restituisco

                                ultima azione cancello il carrello dell'utente

        Sfrutto il fatto che il buffer ha il seguente pattern: "isbn\n isbn\n isbn\n"
    */
    printf("\npre step2\n");
    strcpy(bufferCart, "");

    for (i = 0; i < num_rows; i++) {

        free(singoloISBN);
        singoloISBN = (char*)malloc(MAX_MESSAGE_LENGTH);

            if (i == 0) {
                singoloISBN = strtok(bufferCh, "\n");
            } else {
                singoloISBN = strtok(NULL, "\n");
            }

            int intISBN = atoi(singoloISBN);

            printf("\n\nvalore di singoloISBN:%s | intISBN: %d\n\n", singoloISBN, intISBN);

            if ((isLibroDisponibile(singoloISBN, conninfo)) == 1) {
                
                printf("\n2 -\n");
                aggiornaNumeroLibri(intISBN, conninfo);
                printf("\n3 -\n");
                creaNuovoPrestito(email, intISBN, conninfo);
                printf("\n4 -\n");

            } else {

                printf("\n5 -\n");
                
                if (strcmp(bufferCart, "") == 0) {
                    strcpy(bufferCart, "L'ultima copia dei seguenti libri già è stata presa in prestito: ");
                    strcat(bufferCart, singoloISBN);
                } else {
                    strcat(bufferCart, " ,");
                    strcat(bufferCart, singoloISBN);
                }
                
                printf("\n6 -\n");
            }    
    }
    cancellaCarrelloDiUtente(email, conninfo);
    printf("\n7 -\n");

return bufferCart;
}


int isLibroDisponibile(char * ISBN, char *conninfo)
{ 

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        printf("ild : 3\n"); 
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    printf("ISBN CHE ricevo in is Libro disponibile: %s",ISBN);

    const char *paramValues[1] = { ISBN };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT copieTotali, totcopieprestate FROM libro WHERE isbn = $1",
                                1,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValues, // Valori dei parametri
                                NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                NULL,     // Formato dei parametri (NULL per stringhe)
                                0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    if (PQntuples(res) > 0) {
        char *copieTotaliStr = PQgetvalue(res, 0, 0); // Riga 0, Colonna 0 (copieTotali)
        char *totCopiePrestateStr = PQgetvalue(res, 0, 1); // Riga 0, Colonna 1 (totcopieprestate)

        int copieTotali = atoi(copieTotaliStr);
        int totCopiePrestate = atoi(totCopiePrestateStr);

        PQclear(res);
        PQfinish(conn);

        if (copieTotali > totCopiePrestate)
        {
            return 1;
        }
        else 
        {
            return 0;   
        }

    }
}


void aggiornaNumeroLibri(int ISBN, char *conninfo) 
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    printf("\nAGGNUMLIB -1");
    free(charISBN);
    charISBN = (char *)malloc(12 * sizeof(char));
    sprintf(charISBN, "%d", ISBN);

    //STEP 1: Mi prendo il numero attuale di totcopieprestate
    const char *paramValuesUno[1] = { charISBN };                                           
    PGresult *res = PQexecParams(conn,
                                "SELECT totcopieprestate FROM libro WHERE isbn = $1",
                                1,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValuesUno, // Valori dei parametri
                                NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                NULL,     // Formato dei parametri (NULL per stringhe)
                                0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    printf("\nAGGNUMLIB -2");

    int num_rows = PQntuples(res);
    charNumeroCopie = PQgetvalue(res, 0, 0); 
    numeroCopie = atoi(charNumeroCopie);                                                     
    numeroCopie++;

    printf("\nAGGNUMLIB -3");

    free(charISBN);
    charISBN = (char *)malloc(12 * sizeof(char));

    sprintf(charISBN, "%d", ISBN);

    printf("\nValori in aggiornaNumeroLibti che passo alla query: %s %s . \n",charNumeroCopie, charISBN);

    //STEP 2: Aggiorno il numero di copie
    const char *paramValuesDue[2] = { charNumeroCopie, charISBN };
    res = PQexecParams(conn,
                                 "UPDATE libro SET totcopieprestate = $1 WHERE isbn = $2",
                                 2,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValuesDue, // Valori dei parametri
                                 NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,     // Formato dei parametri (NULL per stringhe)
                                 0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    printf("\nAGGNUMLIB -4");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query finale di aggiorna numero libri: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }
}


void creaNuovoPrestito(char *email, int ISBN, char *conninfo)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    printf("in creaNuovoPrestito\n");

    time_t t = time(NULL);
    printf("-prestito 2 -\n");
    struct tm *localTimeStamp = localtime(&t);
    printf("-prestito 3 -\n");

    int currMese, currGiorno, meseRestituzione;
    char *dataPrestito, *dataRestituzione;

    printf ("pre free");
    free(charISBN); free(dataPrestito); free(dataRestituzione);
    printf ("\n post free");

    dataPrestito = (char*)malloc(MAX_MESSAGE_LENGTH);
    dataRestituzione = (char*)malloc(MAX_MESSAGE_LENGTH);
    charISBN = (char *)malloc(12 * sizeof(char));

    sprintf(charISBN, "%d", ISBN);

    currMese = localTimeStamp->tm_mon + 1;
    currGiorno =  localTimeStamp->tm_mday;
    meseRestituzione = localTimeStamp->tm_mon + 4;

    sprintf(dataPrestito, "%02d/%02d", currMese, currGiorno);
    sprintf(dataRestituzione, "%02d/%02d", meseRestituzione, currGiorno);

    printf("in creaNuovoPrestito - GIORNI\n");
    printf("Data prestito: %s | Data restituzione: %s", dataPrestito, dataRestituzione);

    const char *paramValues[4] = { charISBN, email, dataPrestito, dataRestituzione };
    PGresult *res = PQexecParams(conn,
                                "INSERT INTO prestito(isbnPrestito, emailPrestito, dataPrestito, dataRestituzione) VALUES ($1 $2 $3 $4)",
                                4,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValues, // Valori dei parametri
                                NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                NULL,     // Formato dei parametri (NULL per stringhe)
                                0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query crea Nuovo Prestito\n : %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
    }

}


void cancellaCarrelloDiUtente(char *email, char *conninfo)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

        printf("\nInizio eliminazione carrello.\n");

    const char *paramValues[1] = { email };
    PGresult *res = PQexecParams(conn,
                                "DELETE * FROM carrello WHERE emailCarrello = $1",
                                1,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValues, // Valori dei parametri
                                NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                NULL,     // Formato dei parametri (NULL per stringhe)
                                0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) 
    {
        fprintf(stderr, "Errore durante la query cancellaCarrelloDiUtente : %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
    }
    printf("\nFine eliminazione carrello.\n");
}