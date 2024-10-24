#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

void aggiungiLibroAlCarrello(int socket, char *email, char * ISBN, char *conninfo);
char* checkout(int socket, char *email, char *conninfo);
int isLibroDisponibile(char * ISBN, char *conninfo);
void aggiornaNumeroLibri(int ISBN, char *conninfo);
void creaNuovoPrestito(char *email, int ISBN, char *conninfo);
void cancellaCarrelloDiUtente(char *email, char *conninfo);

#endif
