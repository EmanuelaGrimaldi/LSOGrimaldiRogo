#ifndef UTENTE_H
#define UTENTE_H

#include "define.h" // ?

void registraNuovoUtente(int socket, char *nome, char *email, char *password, char *conninfo);
int loginUtente(int socket, char *email, char *password, char *conninfo);

void accedi(char *email, char *conninfo);
void logout();

int emailValida(char *emailDaVerificare, char *conninfo);
char *getNomeUtente(char *email, char *conninfo);

int getValoreK ( char *conninfo);
void updateValoreK ( char *conninfo, int nuovoK);

#endif