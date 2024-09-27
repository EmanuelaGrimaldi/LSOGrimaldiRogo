#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "carrello.h"
#include "libro.h"
#include "define.h"

#include <libpq-fe.h>
const char *conninfo = "host=localhost port=5432 dbname=mydb user=myuser password=mypassword";

int numeroCopie = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void aggiungiLibroAlCarrello(int socket, char email[MAX_LENGTH], int ISBN)
{
     PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    //Creo ed eseguo la query
    const char *paramValues[2] = { ISBN, email };
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
        return;
    
    // Libera la memoria allocata per il risultato
    PQclear(res);
    PQfinish(conn);

    printf("Elemento aggiunto al carrello con successo!\n");
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void checkout(int socket, char email[MAX_LENGTH])
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    //STEP 1: Mi prendo tutti i libri messi nel carrello da quell'utente
    const char *paramValues[1] = { ISBN };
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
    int tempISBN;

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_fields; col++) {
            tempISBN = PQgetvalue(res, row, col);
            aggiornaNumeroLibri(tempISBN);
            creaNuovoPrestito(email, tempISBN);
            cancellaCarrelloDiUtente(email);
        }
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
int isLibroDisponibile(int isbn)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = { isbn };
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
        numeroCopie = PQgetvalue(res, 0, 0); 
    
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void aggiornaNumeroLibri(int ISBN) 
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    //STEP 1: Mi prendo il numero attuale di totCopieDisponibili
    const char *paramValues1[1] = { ISBN };
    PGresult *res = PQexecParams(conn,
                                "SELECT totCopieDisponibili FROM libro WHERE isbn = $1",
                                1,        // Numero di parametri
                                NULL,     // OID dei parametri (NULL per default)
                                paramValues1, // Valori dei parametri
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
    int numeroCopie = PQgetvalue(res, 0, 0); 
    numeroCopie--;

    //STEP 2: Aggiorno il numero di copie
    const char *paramValues2[2] = { numeroCopie, ISBN };
    PGresult *res = PQexecParams(conn,
                                 "UPDATE libro SET totCopieDisponibili = $1 WHERE isbn = $2",
                                 2,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValues2, // Valori dei parametri
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
}


void creaNuovoPrestito(char email[MAX_LENGTH], int ISBN)
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    time_t t = time(NULL);
    struct tm tempDataPrestito = *localtime(&t);

    tempDataPrestito.tm_year+=1900;
    tempDataPrestito.tm_mon+=1;

    struct tm tempDataRestituzione = (tempDataPrestito.tm_mon+=3);

    const char *paramValues[4] = { ISBN, email, tempDataPrestito, tempDataRestituzione };
    PGresult *res = PQexecParams(conn,
                                "INSERT INTO prestito(isbnPrestito, emailPrestito, dataPrestito, dataRestituzione)
                                 VALUES ($1 $2 $3 $4)",
                                4,        // Numero di parametri
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

}
            
void cancellaCarrelloDiUtente(char email[MAX_LENGTH])
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
}