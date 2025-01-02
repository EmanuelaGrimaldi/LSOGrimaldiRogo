#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include <libpq-fe.h>
#include "libro.h"

char *titolo, *chISBN, *categoria, *charCopieTotali, *charTotCopiePrestate, *bufferPoin, *bufferPoinDeluxe, *charCopieDisponibili, *emailPrestito, *dataPrestito, *dataRestituzione;
int intCopieTotali, intTotCopiePrestate, copieDisponibili, valore, Ipointer;
char stringToAppend[MAX_MESSAGE_LENGTH];

char *cercaLibroByParolaChiave(int socket, char *parolaChiave, char *conninfo)
{
    free(bufferPoin);
    free(chISBN);
    free(titolo);
    free(categoria);
    free(charCopieTotali);
    free(charTotCopiePrestate);
    free(charCopieDisponibili);
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    char queryKey[256]; // Assicurati che sia abbastanza grande per contenere parolaChiave + caratteri jolly
    snprintf(queryKey, sizeof(queryKey), "%%%s%%", parolaChiave);

    const char *paramValues[1] = {queryKey};
    PGresult *res = PQexecParams(conn,
                                 "SELECT * FROM libro WHERE titolo ILIKE $1",
                                 1,           // Numero di parametri
                                 NULL,        // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,        // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,        // Formato dei parametri (NULL per stringhe)
                                 0);          // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    int numeroRighe = PQntuples(res);

    // Stampo tutti i risultati trovati
    if (numeroRighe > 0)
    {
        for (valore = 0; valore < numeroRighe; valore++)
        {
            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, valore, 0));
            snprintf(titolo, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 1));
            snprintf(categoria, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 2));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, valore, 3));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, valore, 4));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili = intCopieTotali - intTotCopiePrestate;

            sprintf(charCopieDisponibili, "%d", copieDisponibili);

            // Aggiungi il libro in coda al buffer dei risultati

            if (valore == 0)
                strcpy(bufferPoin, "Titolo: ");

            else
                strcat(bufferPoin, "Titolo: ");

            strcat(bufferPoin, titolo);
            strcat(bufferPoin, "| ISBN: ");
            strcat(bufferPoin, chISBN);
            strcat(bufferPoin, "| Categoria: ");
            strcat(bufferPoin, categoria);
            strcat(bufferPoin, "| Copie disponibili: ");
            strcat(bufferPoin, charCopieDisponibili);
            strcat(bufferPoin, "\n");
        }
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}

char *cercaLibroByISBN(int socket, char *ISBN, char *conninfo)
{
    free(bufferPoin);
    free(chISBN);
    free(titolo);
    free(categoria);
    free(charCopieTotali);
    free(charTotCopiePrestate);
    free(charCopieDisponibili);

    printf("AAAA\n");

    bufferPoin = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    chISBN = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    titolo = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    categoria = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    charCopieTotali = (char *)malloc(MAX_MESSAGE_LENGTH);
    charTotCopiePrestate = (char *)malloc(MAX_MESSAGE_LENGTH);
    charCopieDisponibili = (char *)malloc(MAX_MESSAGE_LENGTH);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = {ISBN};
    PGresult *res = PQexecParams(conn,
                                 "SELECT * FROM libro WHERE ISBN = $1",
                                 1,           // Numero di parametri
                                 NULL,        // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,        // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,        // Formato dei parametri (NULL per stringhe)
                                 0);          // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    int numeroRighe = PQntuples(res);

    printf("AAAA\n");

    // Stampo tutti i risultati trovati
    if (numeroRighe > 0)
    {

        printf("AAAA\n");

        for (valore = 0; valore < numeroRighe; valore++)
        {
            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, valore, 0));
            snprintf(titolo, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 1));
            snprintf(categoria, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 2));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, valore, 3));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, valore, 4));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili = intCopieTotali - intTotCopiePrestate;

            sprintf(charCopieDisponibili, "%d", copieDisponibili);

            // Aggiungi il libro in coda al buffer dei risultati
            
            printf("AAAA\n");

            if (valore == 0)
                strcpy(bufferPoin, "Titolo: ");

            else
                strcat(bufferPoin, "Titolo: ");

            strcat(bufferPoin, titolo);
            strcat(bufferPoin, "| ISBN: ");
            strcat(bufferPoin, chISBN);
            strcat(bufferPoin, "| Categoria: ");
            strcat(bufferPoin, categoria);
            strcat(bufferPoin, "| Copie disponibili: ");
            strcat(bufferPoin, charCopieDisponibili);
            strcat(bufferPoin, "\n");
        }

    }
    else
    {
        strcpy(bufferPoin, "Non è stato trovato nessun libro con l'isbn da lei inserito.\n");
    }
    
    PQclear(res);
    PQfinish(conn);

    return bufferPoin;

}

char *cercaLibroByCategoria(int socket, char *categoria_x, char *conninfo)
{
    free(bufferPoin);
    free(chISBN);
    free(titolo);
    free(categoria);
    free(charCopieTotali);
    free(charTotCopiePrestate);
    free(charCopieDisponibili);

    bufferPoin = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    chISBN = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    titolo = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    categoria = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    charCopieTotali = (char *)malloc(MAX_MESSAGE_LENGTH);
    charTotCopiePrestate = (char *)malloc(MAX_MESSAGE_LENGTH);
    charCopieDisponibili = (char *)malloc(MAX_MESSAGE_LENGTH);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    const char *paramValues[1] = {categoria_x};
    PGresult *res = PQexecParams(conn,
                                 "SELECT * FROM libro WHERE categoria ILIKE $1",
                                 1,           // Numero di parametri
                                 NULL,        // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,        // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,        // Formato dei parametri (NULL per stringhe)
                                 0);          // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    int numeroRighe = PQntuples(res);

    // Stampo tutti i risultati trovati
    if (numeroRighe > 0)
    {
        for (valore = 0; valore < numeroRighe; valore++)
        {
            // Estrae i dati dalla query
            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, valore, 0));
            snprintf(titolo, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 1));
            snprintf(categoria, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, valore, 2));
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, valore, 3));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, valore, 4));

            intCopieTotali = atoi(charCopieTotali);
            intTotCopiePrestate = atoi(charTotCopiePrestate);

            copieDisponibili = intCopieTotali - intTotCopiePrestate;

            sprintf(charCopieDisponibili, "%d", copieDisponibili);

            // Aggiungi il libro in coda al buffer dei risultati

            if (valore == 0)
                strcpy(bufferPoin, "Titolo: ");

            else
                strcat(bufferPoin, "Titolo: ");

            strcat(bufferPoin, titolo);
            strcat(bufferPoin, "| ISBN: ");
            strcat(bufferPoin, chISBN);
            strcat(bufferPoin, "| Categoria: ");
            strcat(bufferPoin, categoria);
            strcat(bufferPoin, "| Copie disponibili: ");
            strcat(bufferPoin, charCopieDisponibili);
            strcat(bufferPoin, "\n");
        }
    }
    else
    {
        strcpy(bufferPoin, "Non è stato trovato nessun libro con l'isbn da lei inserito.\n");
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}

char *getAllLibri(char *conninfo)
{

    free(bufferPoin);
    free(chISBN);
    free(titolo);
    free(categoria);
    free(charCopieTotali);
    free(charTotCopiePrestate);
    free(charCopieDisponibili);

    bufferPoin = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    chISBN = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    titolo = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    categoria = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    charCopieTotali = (char *)malloc(MAX_MESSAGE_LENGTH);
    charTotCopiePrestate = (char *)malloc(MAX_MESSAGE_LENGTH);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    PGresult *res = PQexec(conn, "SELECT * FROM libro");

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    int numeroRighe = PQntuples(res);

    // Stampo tutti i risultati trovati
    if (numeroRighe > 0)
    {

        for (Ipointer = 0; Ipointer < numeroRighe; Ipointer++)
        {

            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, Ipointer, 0));
            snprintf(titolo, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, Ipointer, 1));
            snprintf(categoria, MAX_MESSAGE_LENGTH * sizeof(char), "%s", PQgetvalue(res, Ipointer, 2));
            printf("\nCategoria:%s\n", categoria);
            snprintf(charCopieTotali, sizeof(charCopieTotali), "%s", PQgetvalue(res, Ipointer, 3));
            snprintf(charTotCopiePrestate, sizeof(charTotCopiePrestate), "%s", PQgetvalue(res, Ipointer, 4));

            if (Ipointer == 0)
            {
                strcpy(bufferPoin, "Titolo: ");
            }
            else
            {
                strcat(bufferPoin, "Titolo: ");
            }

            strcat(bufferPoin, titolo);
            strcat(bufferPoin, "| ISBN: ");
            strcat(bufferPoin, chISBN);
            strcat(bufferPoin, "| Categoria: ");
            strcat(bufferPoin, categoria);
            strcat(bufferPoin, "| Copie totali: ");
            strcat(bufferPoin, charCopieTotali);
            strcat(bufferPoin, "| Copie prese in prestito: ");
            strcat(bufferPoin, charTotCopiePrestate);
            strcat(bufferPoin, "\n");
        }
    }
    else
    {
        strcpy(bufferPoin, "Errore query in getAllLibri.\n");
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoin;
}

char *getAllPrestiti(char *conninfo)
{

    printf("\nSono in get all prestiti");

    free(bufferPoinDeluxe);
    free(chISBN);
    free(emailPrestito);
    free(dataPrestito);
    free(dataRestituzione);

    bufferPoinDeluxe = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char) * 10);
    chISBN = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    emailPrestito = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    dataPrestito = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    dataRestituzione = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    PGresult *res = PQexec(conn, "SELECT * FROM prestito");

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    int numeroRighe = PQntuples(res);

    if (numeroRighe > 0)
    {

        for (Ipointer = 0; Ipointer < numeroRighe; Ipointer++)
        {

            snprintf(chISBN, sizeof(chISBN), "%s", PQgetvalue(res, Ipointer, 0));
            snprintf(emailPrestito, sizeof(emailPrestito), "%s", PQgetvalue(res, Ipointer, 1));
            snprintf(dataPrestito, sizeof(dataPrestito), "%s", PQgetvalue(res, Ipointer, 2));
            snprintf(dataRestituzione, sizeof(dataRestituzione), "%s", PQgetvalue(res, Ipointer, 3));

            if (Ipointer == 0)
            {
                strcpy(bufferPoinDeluxe, "ISBN: ");
            }
            else
            {
                strcat(bufferPoinDeluxe, "ISBN: ");
            }

            strcat(bufferPoinDeluxe, chISBN);
            strcat(bufferPoinDeluxe, "| Email cliente: ");
            strcat(bufferPoinDeluxe, emailPrestito);
            strcat(bufferPoinDeluxe, "| Data prestito: ");
            strcat(bufferPoinDeluxe, dataPrestito);
            strcat(bufferPoinDeluxe, "| Data restituzione: ");
            strcat(bufferPoinDeluxe, dataRestituzione);
            strcat(bufferPoinDeluxe, "\n");
        }
    }
    else
    {
        strcpy(bufferPoinDeluxe, "Errore query in getAllPrestiti.\n");
    }

    PQclear(res);
    PQfinish(conn);

    return bufferPoinDeluxe;
}
