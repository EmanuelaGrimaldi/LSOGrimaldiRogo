#ifndef LIBRO_H
#define LIBRO_H

#include "define.h"

char *cercaLibroByTitolo(int socket, char *parolaChiave, char *conninfo);
char *cercaLibroByISBN(int socket, int ISBN, char *conninfo);

#endif
