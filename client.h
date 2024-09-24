#ifndef CLIENT_H
#define CLIENT_H

void menuGuest(int socket);
void menuUser(int socket);
void login(int socket);
void registerUser(int socket);
void searchBook(int socket);
void addToCart(int socket);
void checkout(int socket);
void svuotaCarrelloLibri();
void accedi();
void disaccedi();

#define MAX_MASSAGE_LENGTH 1024
#define CONNESSO 1
#define NON_CONNESSO 0
#endif
