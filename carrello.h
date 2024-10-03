#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

void aggiungiLibroAlCarrello(int socket, char *email, int ISBN, char *conninfo);
void checkout(int socket, char *email, char *conninfo);
int isLibroDisponibile(int isbn, char *conninfo);
void aggiornaNumeroLibri(int ISBN, char *conninfo);
void creaNuovoPrestito(char *email, int ISBN, char *conninfo);
void cancellaCarrelloDiUtente(char *email, char *conninfo);

#endif
