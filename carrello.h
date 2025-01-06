#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

int aggiungiLibroAlCarrello(int socket, char *email, char *ISBN, char *conninfo);
void aggiornaNumeroLibri(int ISBN, char *conninfo);

char *getAllLibriInCarrello(char *conninfo, char *emaill);
char *getAllPrestiti(char *conninfo, char *emaill);

int isLibroDisponibile(char *ISBN, char *conninfo);
int isNumeroLibriCarrelloMaggioreDiK(char *email, char *conninfo);

void creaNuovoPrestito(char *email, int ISBN, char *conninfo);
void cancellaCarrelloDiUtente(char *email, char *conninfo);

char *checkout(int socket, char *email, char *conninfo);

#endif
