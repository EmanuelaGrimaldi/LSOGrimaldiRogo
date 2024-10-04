#ifndef CLIENT_H
#define CLIENT_H

#include "define.h" // ?

int main();
void menuGuest(int socket);
void menuUser(int socket);
void funzioneLogin(int socket);
void funzioneRegister(int socket);
void funzioneSearchParolaChiave (int socket);
void funzioneSearchISBN (int socket);
void funzioneAddToCart(int socket);

#endif
