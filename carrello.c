#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"
#include <libpq-fe.h>

char *puntatoreCharISBN, *charNumeroCopie, *charISBN;
int numeroCopie;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void aggiungiLibroAlCarrello(int socket, char *email, int ISBN, char *conninfo)
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

    if (isLibroDisponibile(ISBN, conninfo) == RISPOSTA_INVALIDA){
        printf("Non vi sono copie disponibili per questo libro.\n\n");
    } else {

        //Creo ed eseguo la query
        const char *paramValues[2] = { puntatoreCharISBN, email };                                                 
        PGresult *res = PQexecParams(conn,
                                    "INSERT INTO carrello (isbnCarrello, emailCarrello) VALUES ($1, $2)",
                                    2,        // Numero di parametri
                                    NULL,     // OID dei parametri (NULL per default)
                                    paramValues, // Valori dei parametri
                                    NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                    NULL,     // Formato dei parametri (NULL per stringhe)
                                    0);       // Formato del risultato (0 = testo)

        // Verifica se l'operazione è andata a buon fine
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Errore durante l'inserimento: %s", PQerrorMessage(conn));
        } else {
            printf("Inserimento del nuovo utente riuscito!\n");
        }

            PQclear(res);
            PQfinish(conn);
            free(puntatoreCharISBN);
            return;
        
        // Libera la memoria allocata per il risultato
        PQclear(res);
        PQfinish(conn);

        printf("Elemento aggiunto al carrello con successo!\n");
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void checkout(int socket, char *email, char *conninfo)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    //STEP 1: Mi prendo tutti i libri messi nel carrello da quell'utente
    const char *paramValues[1] = { email };
    PGresult *res = PQexecParams(conn,
                                "SELECT * FROM carrello WHERE emailCarrello = $1",
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
        return;
    }

    /*STEP 2: per ogni libro:   aggiorno numero copie disponibili
                                creo un nuovo oggetto prestito
                                cancello le righe dal carrello
    */
    char * charPointerISBN;
    int num_rows = PQntuples(res),  num_fields = 2;

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_fields; col++) {

            charPointerISBN = PQgetvalue(res, row, col);
            int intISBN = atoi(charPointerISBN);

            aggiornaNumeroLibri(intISBN, conninfo);
            creaNuovoPrestito(email, intISBN, conninfo);
            cancellaCarrelloDiUtente(email, conninfo);
        }
    }

}


int isLibroDisponibile(int isbn, char *conninfo)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    puntatoreCharISBN = (char *)malloc(12 * sizeof(char));
    sprintf(puntatoreCharISBN, "%d", isbn);

    const char *paramValues[1] = { puntatoreCharISBN };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT totCopieDisponibili FROM libro WHERE isbn = $1",
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

    int num_rows = PQntuples(res);
        
    if (num_rows == 1) 
    {
        charNumeroCopie = PQgetvalue(res, 0, 0); 
        numeroCopie = atoi(charNumeroCopie);
    
        if (numeroCopie == 0)
        {
            printf("Non vi sono copie disponibili attualmente per questo libro.\n");
            PQclear(res);
            PQfinish(conn);
            return RISPOSTA_INVALIDA;
        }
        else 
        {
           printf("Vi sono %d copie disponibili attualmente per questo libro.\n", numeroCopie); 
        }

        PQclear(res);
        PQfinish(conn);
        return RISPOSTA_VALIDA;
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