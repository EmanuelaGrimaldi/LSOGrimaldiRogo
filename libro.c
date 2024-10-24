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
    free(bufferPoin); free(chISBN); free(titolo); free(charCopieTotali); free(charTotCopiePrestate); free(charCopieDisponibili);

    bufferPoin = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    chISBN = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    titolo = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    charCopieTotali = (char*)malloc(MAX_MESSAGE_LENGTH);
    charTotCopiePrestate = (char*)malloc(MAX_MESSAGE_LENGTH);
    charCopieDisponibili  = (char*)malloc(MAX_MESSAGE_LENGTH);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    char queryKey[256];  // Assicurati che sia abbastanza grande per contenere parolaChiave + caratteri jolly
    snprintf(queryKey, sizeof(queryKey), "%%%s%%", parolaChiave);

    const char *paramValues[1] = { queryKey };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT * FROM libro WHERE titolo ILIKE $1",
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
            snprintf(titolo, MAX_MESSAGE_LENGTH*sizeof(char), "%s", PQgetvalue(res, valore, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, valore, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, valore, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;


            sprintf(charCopieDisponibili, "%d", copieDisponibili); 

            // Aggiungi il libro in coda al buffer dei risultati

            if ( valore == 0 ) {
                    strcpy(bufferPoin, "Titolo: ");
                    strcat(bufferPoin, titolo);
                    strcat(bufferPoin, "| ISBN: ");
                    strcat(bufferPoin, chISBN);
                    strcat(bufferPoin, "| Copie disponibili: ");
                    strcat(bufferPoin, charCopieDisponibili);
                    strcat(bufferPoin, "\n");
            } else {
                    strcat(bufferPoin, "Titolo: ");
                    strcat(bufferPoin, titolo);
                    strcat(bufferPoin, "| ISBN: ");
                    strcat(bufferPoin, chISBN);
                    strcat(bufferPoin, "| Copie disponibili: ");
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
    free(bufferPoin); free(chISBN); free(titolo); free(charCopieTotali); free(charTotCopiePrestate); free(charCopieDisponibili);

    bufferPoin = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    chISBN = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    titolo = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
    charCopieTotali = (char*)malloc(MAX_MESSAGE_LENGTH);
    charTotCopiePrestate = (char*)malloc(MAX_MESSAGE_LENGTH);
    charCopieDisponibili  = (char*)malloc(MAX_MESSAGE_LENGTH);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = { ISBN };                                            
    PGresult *res = PQexecParams(conn,
                                "SELECT * FROM libro WHERE ISBN = $1",
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
            snprintf(titolo, MAX_MESSAGE_LENGTH*sizeof(char), "%s", PQgetvalue(res, 0, 1));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, 0, 2));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, 0, 3));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili =  intCopieTotali - intTotCopiePrestate;
            sprintf(charCopieDisponibili, "%d", copieDisponibili); 
  
                strcpy(bufferPoin, "Titolo: ");
                strcat(bufferPoin, titolo);
                strcat(bufferPoin, "| ISBN: ");
                strcat(bufferPoin, chISBN);
                strcat(bufferPoin, "| Copie disponibili: ");
                strcat(bufferPoin, charCopieDisponibili);
                strcat(bufferPoin, "\n");
            
    } else {
            strcpy(bufferPoin, "Non è stato trovato nessun libro con l'isbn da lei inserito.\n");
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}
