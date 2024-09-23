#ifndef CARRELLO_H
#define CARRELLO_H

void addBookToCart(int socket, const char *userID);
void checkout(int socket, const char *userID);
int isBookAvailable(const char *bookTitle);
void borrowBook(const char *userID, char *bookTitle);

#endif
