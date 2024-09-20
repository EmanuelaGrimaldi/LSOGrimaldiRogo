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
void funzioneCarrello();
int checkout();

void funzioneCarrello() {

    /*
    crea carrello;
    ogni volta che l'user aggiunge qualcosa la aggiungiamo alla lista
    chiamiamo checkout
    cancella carrello
    */

}


int checkout(){
    /*
    Controllo checkout se il libro è ancora disponibile 
                            se no, messaggio
							se si, prestito effettuato con successo!
                0 = false
                1 = true

                CLIENT: libro.aggiornaNumeroCopieDisponibili(ISBN);
    */
   return 0;
}