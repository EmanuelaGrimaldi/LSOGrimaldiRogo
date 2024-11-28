#ifndef CLIENT_H
#define CLIENT_H

#include "define.h" // ?

int main();
void menuGuest(int socket);
void menuUser(int socket);
void menuAdmin(int socket);

void funzioneLogin(int socket);
void funzioneRegister(int socket);
void funzioneCheckout(int socket);

void funzioneSearchParolaChiave(int socket);
void funzioneSearchISBN(int socket);
void funzioneSearchCategoria(int socket);

void funzioneAddToCart(int socket);

void funzioneElencoLibri(int socket);
void funzioneElencoPrestiti(int socket);

void funzioneModificaK(int socket);

#endif
