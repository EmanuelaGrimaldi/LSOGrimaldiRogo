#ifndef UTENTE_H
#define UTENTE_H

#include "define.h"; // ?

void registerUser(int socket, char name[100], char email[100], char password[30]);
int loginUser(int socket, char *ID, char password[30]);

void accedi(char *email);
void disAccedi();
void setEmailUser(char *server_reply_email);

int existUser(char *user);
int emailValida(char *email);
int pwdValida(char *pwd);

#endif