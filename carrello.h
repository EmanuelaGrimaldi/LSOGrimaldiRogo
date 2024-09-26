#ifndef CARRELLO_H
#define CARRELLO_H

#include "define.h" // ?

void aggiungiLibroAlCarrello(int socket, char email[MAX_LENGTH], int ISBN);
void checkout(int socket, char email[MAX_LENGTH]);
int isLibroDisponibile(int isbn);
void aggiornaNumeroLibri(int ISBN);
void creaNuovoPrestito(char email[MAX_LENGTH], int ISBN);
void cancellaCarrelloDiUtente(char email[MAX_LENGTH]);


#endif
