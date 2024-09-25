#ifndef CLIENT_H
#define CLIENT_H

#include "define.h" // ?

void menuGuest(int socket);
void menuUser(int socket);
void login(int socket);
void registerUser(int socket);
void searchBook(int socket);
void addToCart(int socket);
void checkout(int socket);

#endif
