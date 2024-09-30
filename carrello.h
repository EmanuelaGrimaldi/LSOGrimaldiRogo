#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

void aggiungiLibroAlCarrello(int socket, char *email, int ISBN);
void checkout(int socket, char *email);
int isLibroDisponibile(int isbn);
void aggiornaNumeroLibri(int ISBN);
void creaNuovoPrestito(char *email, int ISBN);
void cancellaCarrelloDiUtente(char *email);


#endif
