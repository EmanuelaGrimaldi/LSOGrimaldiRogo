#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"
#include "define.h"

#include <libpq-fe.h>
const char *conninfo = "host=localhost port=5432 dbname=mydb user=myuser password=mypassword";

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void cercaLibroByTitolo(int socket, char parolaChiave[MAX_LENGTH]) 
{

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    //Creo ed eseguo la query
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT * FROM libro WHERE titolo LIKE '%s'",parolaChiave);
    PGresult *res = PQexec(conn, query);

    // Controlla il risultato della query
    if (PQresultStatus(res) != PGRES_COMMAND_OK) 
    {
        fprintf(stderr, "Errore nell'inserimento dell'utente: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    int num_rows = PQntuples(res), 
        num_fields = PQnfields(res), 
        row, col;
    
    //Stampo tutti i risultati trovati
    if (num_rows > 0)
    {
        printf("\nEcco tutti i libri che corrispondono alla tua ricerca:\n");
        for (row = 0; row < num_rows; row++) 
        {
            for (col = 0; col < num_fields; col++) 
            {
              printf("%s\t", PQgetvalue(res, row, col));
            }
            printf("\n");
        }
    }

    PQclear(res);
    PQfinish(conn);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void cercaLibroByISBN(int socket, int ISBN) 
{

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    //Creo ed eseguo la query
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT * FROM libro WHERE isbn LIKE %d",ISBN);
    PGresult *res = PQexec(conn, query);

    // Controlla il risultato della query
    if (PQresultStatus(res) != PGRES_COMMAND_OK) 
    {
        fprintf(stderr, "Errore nell'inserimento dell'utente: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    int num_rows = PQntuples(res), 
        num_fields = PQnfields(res), 
        row, col;
    
    //Stampo tutti i risultati trovati
    if (num_rows > 0)
    {
        printf("\nEcco tutti i libri che corrispondono alla tua ricerca:\n");
        for (row = 0; row < num_rows; row++) 
        {
            for (col = 0; col < num_fields; col++) 
            {
              printf("%s\t", PQgetvalue(res, row, col));
            }
            printf("\n");
        }
    }

    PQclear(res);
    PQfinish(conn);
}
