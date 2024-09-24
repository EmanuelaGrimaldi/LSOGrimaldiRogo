#ifndef UTENTE_H
#define UTENTE_H

void registerUser(int socket, char name[100], char email[100], char password[30]);
int loginUser(int socket, char *ID, char password[30]);

#endif