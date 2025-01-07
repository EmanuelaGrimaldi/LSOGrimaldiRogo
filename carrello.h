#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

char *getAllLibriInCarrello(char *conninfo, char *email);
int aggiungiLibroAlCarrello(int socket, char *email, char *ISBN, char *conninfo);

char *checkout(int socket, char *email, char *conninfo);
int isLibroDisponibile(char *ISBN, char *conninfo);

void aggiornaNumeroLibri(int ISBN, char *conninfo);
void creaNuovoPrestito(char *email, int ISBN, char *conninfo);

int isNumeroLibriCarrelloMaggioreDiK(char *email, char *conninfo);
void cancellaCarrelloDiUtente(char *email, char *conninfo);


#endif
