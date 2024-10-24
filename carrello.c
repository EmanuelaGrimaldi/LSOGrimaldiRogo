#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"
#include <libpq-fe.h>

char *puntatoreCharISBN, *charNumeroCopie, *charISBN, *bufferCart;
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

    //Mi salvo nel buffer tutti i libri
    if (num_rows > 0)
    for (i = 0; i < num_rows; i++) {

            snprintf(charISBN, sizeof(charISBN), "%s", PQgetvalue(res, i, 0));

            int len = strlen(bufferCh);
            snprintf(toAppend, 10,"%s\n", charISBN); //Qui ho impostato il pattern del buffer
            strcpy(bufferCh + len, toAppend);
    }


    /*STEP 2: per ogni libro:   mi prendo il singolo ISBN
                                controllo ci sia ancora una copia disponibile
                                se c'è, la prendo io + creo un prestito
                                se non c'è, aggiungo l'isbn nell'avviso che restituisco

                                ultima azione cancello il carrello dell'utente

        Sfrutto il fatto che il buffer ha il seguente pattern: "isbn\n isbn\n isbn\n"
    */

    bzero(bufferCart, MAX_MESSAGE_LENGTH);
    char *singoloISBN;

    for (i = 0; i < num_rows; i++) {

            if (i == 0) {
                singoloISBN = strtok(bufferCh, "\n");
            } else {
                singoloISBN = strtok(NULL, "\n");
            }

            int intISBN = atoi(singoloISBN);

            if (isLibroDisponibile(singoloISBN, conninfo)) {

                aggiornaNumeroLibri(intISBN, conninfo);
                creaNuovoPrestito(email, intISBN, conninfo);

            } else {
                
                if (strcmp(bufferCart, "") == 0) {
                    strcpy(bufferCart, "L'ultima copia dei seguenti libri già è stata presa in prestito: ");
                    strcat(bufferCart, singoloISBN);
                } else {
                    strcat(bufferCart, " ,");
                    strcat(bufferCart, singoloISBN);
                }
                
            }
            singoloISBN = strtok(NULL, "\n");      
    }
    cancellaCarrelloDiUtente(email, conninfo);

return bufferCart;
}


int isLibroDisponibile(char * ISBN, char *conninfo)
{
    printf("ild : 1\n"); 

    PGconn *conn = PQconnectdb(conninfo);

    printf("ild : 2\n"); 

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
        char *copieTotaliStr = PQgetvalue(res, 0, 0); // Riga 0, Colonna 2 (copieTotali)
        char *totCopiePrestateStr = PQgetvalue(res, 0, 1); // Riga 0, Colonna 3 (totcopieprestate)

        int copieTotali = atoi(copieTotaliStr);
        int totCopiePrestate = atoi(totCopiePrestateStr);

        PQclear(res);
        PQfinish(conn);

        if (copieTotali > totCopiePrestate)
        {
            return RISPOSTA_VALIDA;
        }
        else 
        {
            return RISPOSTA_INVALIDA;   
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

    puntatoreCharISBN = (char *)malloc(12 * sizeof(char));
    sprintf(puntatoreCharISBN, "%d", ISBN);

    //STEP 1: Mi prendo il numero attuale di totCopieDisponibili
    const char *paramValuesUno[1] = { puntatoreCharISBN };                                           
    PGresult *res = PQexecParams(conn,
                                "SELECT totCopieDisponibili FROM libro WHERE isbn = $1",
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

    int num_rows = PQntuples(res);
    charNumeroCopie = PQgetvalue(res, 0, 0); 
    numeroCopie = atoi(charNumeroCopie);                                                     
    numeroCopie--;

    puntatoreCharISBN = (char *)malloc(12 * sizeof(char));
    sprintf(puntatoreCharISBN, "%d", ISBN);

    //STEP 2: Aggiorno il numero di copie
    const char *paramValuesDue[2] = { charNumeroCopie, puntatoreCharISBN };
    res = PQexecParams(conn,
                                 "UPDATE libro SET totCopieDisponibili = $1 WHERE isbn = $2",
                                 2,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValuesDue, // Valori dei parametri
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

    time_t t = time(NULL);
    struct tm *localTimeStamp = localtime(&t);

    int currMese, currGiorno, meseRestituzione;
    char *dataPrestito, *dataRestituzione;
    
    puntatoreCharISBN = (char *)malloc(12 * sizeof(char));
    sprintf(puntatoreCharISBN, "%d", ISBN);

    currMese = localTimeStamp->tm_mon + 1;
    currGiorno =  localTimeStamp->tm_mday;
    meseRestituzione = localTimeStamp->tm_mon + 4;

    sprintf(dataPrestito, "%02d/%02d", currMese, currGiorno);
    sprintf(dataRestituzione, "%02d/%02d", meseRestituzione, currGiorno);

    const char *paramValues[4] = { puntatoreCharISBN, email, dataPrestito, dataRestituzione };
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
    free(puntatoreCharISBN);

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
    free(puntatoreCharISBN);
}