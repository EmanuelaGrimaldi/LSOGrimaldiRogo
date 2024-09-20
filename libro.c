#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXLEN 30

void ricercaTramiteISBN();
void ricercaTramiteTitolo ();
void aggiornaNumeroCopieDisponibili ();

typedef struct _BOOK {
    char Titolo[MAXLEN];
    int ISBN;
    char autore[MAXLEN];
    char casaEditrice[MAXLEN];
    int copieTotali;
    int copieDisponibili;
} LIBRO;


void ricercaTramiteISBN (){
    //Apro file di testo "ListaLibri" e controllo se c'è un libro con lo stesso ISBN inserito da tastiera.
}

void ricercaTramiteTitolo (){
    //Apro file di testo "ListaLibri" e controllo se c'è un libro con lo stesso titolo inserito da tastiera.
}

void aggiornaNumeroCopieDisponibili () {
    /*
    1) Apro file di testo "ListaLibri" e aggiorna tot copie disponibili
    2) Apre il file di testo ISBN.txt e aggiorna: 
                                totCopieDisponibili
                                totCopiePrestate
                                nuovo gruppo di metadati con ID utente, nome utente, currdate e data di restituzione.
    */
}