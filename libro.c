#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libro.h"
#include "define.h"
#include <libpq-fe.h>

char *titolo, *chISBN, *charCopieTotali, *charTotCopiePrestate, *bufferPoin, *charCopieDisponibili;
int intCopieTotali, intTotCopiePrestate, copieDisponibili, valore;
char stringToAppend[MAX_MESSAGE_LENGTH];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MODIFICATA SECONDO NUOVA LOGICA OK!!
char *cercaLibroByParolaChiave(int socket, char *parolaChiave, char *conninfo)
{
    bzero(bufferPoin, MAX_MESSAGE_LENGTH);
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = { parolaChiave };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT * FROM libro WHERE titolo LIKE '$1'",
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

    // Stampo tutti i risultati trovati
    if (num_rows > 0){
        for (valore = 0; valore < num_rows; valore++) {

            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, valore, 0));
            snprintf(titolo, sizeof(titolo), "%s", PQgetvalue(res, valore, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, valore, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, valore, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;
            sprintf(charCopieDisponibili, "%d", copieDisponibili); 

            // Aggiungi il libro in coda al buffer dei risultati

            if ( valore == 0 ) {
                    strcpy(bufferPoin, titolo);
                    strcat(bufferPoin, ",");
                    strcat(bufferPoin, chISBN);
                    strcat(bufferPoin, ",");
                    strcat(bufferPoin, charCopieDisponibili);
                    strcat(bufferPoin, "\n");
                } else {
                    strcat(bufferPoin, titolo);
                    strcat(bufferPoin, ",");
                    strcat(bufferPoin, chISBN);
                    strcat(bufferPoin, ",");
                    strcat(bufferPoin, charCopieDisponibili);
                    strcat(bufferPoin, "\n");
                }
        }
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MODIFICATA SECONDO NUOVA LOGICA OK!!
char *cercaLibroByISBN(int socket, char* ISBN, char *conninfo)
{
    bzero(bufferPoin, MAX_MESSAGE_LENGTH);
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = { ISBN };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT * FROM libro WHERE ISBN = '$1'",
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

    // Stampo tutti i risultati trovati
    if (num_rows > 0) {

            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, 0, 0));
            snprintf(titolo, sizeof(titolo), "%s", PQgetvalue(res, 0, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, 0, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, 0, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;
            sprintf(charCopieDisponibili, "%d", copieDisponibili); 
  
            strcpy(bufferPoin, titolo);
            strcat(bufferPoin, ",");
            strcat(bufferPoin, chISBN);
            strcat(bufferPoin, ",");
            strcat(bufferPoin, charCopieDisponibili);
            strcat(bufferPoin, "\n");
            
    } else {
            strcpy(bufferPoin, "Non è stato trovato nessun libro con l'isbn da lei inserito.\n");
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}
