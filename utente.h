#ifndef UTENTE_H
#define UTENTE_H

#include "define.h" // ?

void registraNuovoUtente(int socket, char nome[MAX_LENGTH], char email[MAX_LENGTH], char password[30]);
int loginUtente(int socket, char email[MAX_LENGTH], char password[MAX_LENGTH]);

void accedi(char email[MAX_LENGTH]);
void disAccedi();

int emailValida(char emailDaVerificare[MAX_LENGTH]);
char getNomeUtente (char email[MAX_LENGTH]);

#endif