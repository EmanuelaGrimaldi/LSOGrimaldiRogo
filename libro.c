#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"
#include "define.h"
#include <libpq-fe.h>

char *titolo, *chISBN, *charCopieTotali, *charTotCopiePrestate, *bufferPoin;
int intCopieTotali, intTotCopiePrestate, copieDisponibili;
char stringToAppend[MAX_MESSAGE_LENGTH];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MODIFICATA SECONDO NUOVA LOGICA OK!!
char *cercaLibroByTitolo(int socket, char *parolaChiave, char *conninfo)
{
    // QUESTA FUNZIONE STAMPA SU STDOUTPUT NON RESTITUISCE QUELLO CHE TROVA

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    // Creo ed eseguo la query
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM libro WHERE titolo LIKE '%s'", parolaChiave);
    PGresult *res = PQexec(conn, query);

    // Controlla il risultato della query
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Errore nell'inserimento dell'utente: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    int num_rows = PQntuples(res);

    // Stampo tutti i risultati trovati
    if (num_rows > 0)
    for (int i = 0; i < num_rows; i++) {

            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, i, 0));
            snprintf(titolo, sizeof(titolo), "%s", PQgetvalue(res, i, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, i, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, i, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;

            // Aggiungi il libro in coda al buffer dei risultati
            int len = strlen(bufferPoin);
            snprintf(stringToAppend, 100,"ISBN: %s | Titolo: %s | Copie Disponibili: %d | CopieTotali: %d\n", chISBN, titolo, copieDisponibili, intCopieTotali);
            strcpy(bufferPoin + len, stringToAppend);
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MODIFICATA SECONDO NUOVA LOGICA OK!!
char *cercaLibroByISBN(int socket, int ISBN, char *conninfo)
{

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    // Creo ed eseguo la query
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM libro WHERE isbn LIKE %d", ISBN);
    PGresult *res = PQexec(conn, query);

    // Controlla il risultato della query
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Errore nell'inserimento dell'utente: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    int num_rows = PQntuples(res);

    // Stampo tutti i risultati trovati
    if (num_rows > 0)
    for (int i = 0; i < num_rows; i++) {

            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, i, 0));
            snprintf(titolo, sizeof(titolo), "%s", PQgetvalue(res, i, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, i, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, i, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;

            // Aggiungi il libro in coda al buffer dei risultati
            int len = strlen(bufferPoin);
            snprintf(stringToAppend, 100,"ISBN: %s | Titolo: %s | Copie Disponibili: %d | CopieTotali: %d\n", chISBN, titolo, copieDisponibili, intCopieTotali);
            strcpy(bufferPoin + len, stringToAppend);
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}
