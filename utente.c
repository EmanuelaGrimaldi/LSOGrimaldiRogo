#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utente.h"

#include <libpq-fe.h>
const char *conninfo = "host=localhost port=5432 dbname=mydb user=myuser password=mypassword";

char user_name[MAX_LENGTH];
char user_email[MAX_LENGTH];
int client_connesso, risposta;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
void registraNuovoUtente(int socket, char nome[MAX_LENGTH], char email[MAX_LENGTH], char password[30])
{    
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }
    //Creo ed eseguo la query
    const char *paramValues[3] = { nome, email, password };
    PGresult *res = PQexecParams(conn,
                                 "INSERT INTO utente (nome, email, password) VALUES ($1, $2, $3)",
                                 3,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,     // Formato dei parametri (NULL per stringhe)
                                 0);       // Formato del risultato (0 = testo)

    // Verifica se l'operazione è andata a buon fine
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Errore durante l'inserimento: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return;
    }

    PQclear(res);
    PQfinish(conn);

    printf("Utente inserito con successo!\n");
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
int loginUtente(int socket, char email[MAX_LENGTH], char password[MAX_LENGTH])
{
    recv(socket, email, MAX_LENGTH, 0);
    recv(socket, password, MAX_LENGTH, 0);

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    // Prepara la query SQL per cercare l'utente con l'email e la password forniti
    const char *paramValues[2] = { email, password };
    PGresult *res = PQexecParams(conn,
                                 "SELECT * FROM utenti WHERE email = $1 AND password = $2",
                                 2,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,     // Formato dei parametri (NULL per stringhe)
                                 0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    // Verifica se c'è almeno una riga di risultato (cioè l'utente esiste)
    int num_rows = PQntuples(res);
    if (num_rows == 1) {
        accedi(email);
        printf("Login riuscito!\n");
        PQclear(res);
        PQfinish(conn);
        return 1;  // Successo
    } else {
        printf("Email o password non validi!\n");
        PQclear(res);
        PQfinish(conn);
        return 0;  // Fallimento
    }
}

//OK!!
void accedi(char email[MAX_LENGTH])
{
    client_connesso = CONNESSO;
    strcpy(user_name, getNomeUtente(email));
    strcpy(user_email, email);
}

//OK!!
void disAccedi()
{
    client_connesso = NON_CONNESSO;
    strcpy(user_name, "");
    strcpy(user_email, "");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
int emailValida(char emailDaVerificare[MAX_LENGTH])
{
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    // Prepara la query SQL per cercare l'utente con l'email e la password forniti
    const char *paramValues[1] = { email};
    PGresult *res = PQexecParams(conn,
                                 "SELECT * FROM utenti WHERE email = $1",
                                 1,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,     // Formato dei parametri (NULL per stringhe)
                                 0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    // Verifica se c'è almeno una riga di risultato
    int num_rows = PQntuples(res);
    if (num_rows == 1) {
        printf("Email non valida, è già presente nel nostro sistema!\n");
        PQclear(res);
        PQfinish(conn);
        return RISPOSTA_INVALIDA;
    }

    return RISPOSTA_VALIDA;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DATABASEIZZATO - NOT OK
char getNomeUtente (char email[MAX_LENGTH])
{
    char line[256];

    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) 
    {
        fprintf(stderr, "Connessione al database fallita: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 0;
    }

    // Prepara la query SQL per cercare l'utente con l'email e la password forniti
    const char *paramValues[1] = { email};
    PGresult *res = PQexecParams(conn,
                                 "SELECT nome FROM utenti WHERE email = $1",
                                 1,        // Numero di parametri
                                 NULL,     // OID dei parametri (NULL per default)
                                 paramValues, // Valori dei parametri
                                 NULL,     // Lunghezza dei parametri (NULL per stringhe)
                                 NULL,     // Formato dei parametri (NULL per stringhe)
                                 0);       // Formato del risultato (0 = testo)

    // Verifica il risultato della query
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore durante la query: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }

    //Prendo il valore del risultato e lo restituisco
    int num_rows = PQntuples(res);
    if (num_rows == 1) {
        char risultatoNome[MAX_LENGTH] = PQgetvalue(res, 0, 0); 
        return risultatoNome;
    }

    return NULL;
}
