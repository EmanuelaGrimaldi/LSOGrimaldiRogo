#ifndef LIBRO_H
#define LIBRO_H

#include "define.h"

void cercaLibroByTitolo(int socket, char *parolaChiave, char *conninfo);
void cercaLibroByISBN(int socket, int ISBN, char *conninfo);

#endif
