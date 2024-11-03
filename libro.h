#ifndef LIBRO_H
#define LIBRO_H

#include "define.h"

char *cercaLibroByParolaChiave(int socket, char *parolaChiave, char *conninfo);
char *cercaLibroByISBN(int socket, char* ISBN, char *conninfo);
char *getAllLibri(char *conninfo);
char *getAllPrestiti(char *conninfo);

#endif
