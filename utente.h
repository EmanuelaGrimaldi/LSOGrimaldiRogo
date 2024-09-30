#ifndef UTENTE_H
#define UTENTE_H

#include "define.h" // ?

void registraNuovoUtente(int socket, char *nome, char *email, char *password);
int loginUtente(int socket, char *email, char *password);

void accedi(char *email);
void disAccedi();

int emailValida(char *emailDaVerificare);
char *getNomeUtente (char *email);

#endif