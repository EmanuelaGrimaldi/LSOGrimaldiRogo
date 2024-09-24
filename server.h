#ifndef SERVER_H
#define SERVER_H

void handleClient(int socket);
int controlliUser(char *name, char *email, char *password);
int controlloUtente(char *email, char *password);
int existUser(char *user);
int emailValida(char *email);
int pwdValida(char *pwd);
char *accedi();

#define MAX_MESSAGE_LENGTH 1024
#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_PWD_LENGTH 30
#define RISPOSTA_VALIDA 1
#define RISPOSTA_INVALIDA 0
#endif
