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

void registrazioneUtente ();
void accessoUtente ();

typedef struct _USER {
    char nome[MAXLEN];
    char IDEmail[MAXLEN];
    char listaLibriPrestati[MAXLEN][MAXLEN];
    int numeroLibriPresiInPrestito;
} UTENTE;


void registrazioneUtente (){
    //Prendi dati da tastiera e aggiungi nuovo rigo in file di testo "ListaUtenti"
}

void accessoUtente (){
    //Prendi i dati da tastiera e verifica sia presente nel file di testo "ListaUtenti"
}