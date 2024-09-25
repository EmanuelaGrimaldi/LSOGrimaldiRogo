#ifndef SERVERS_H
#define SERVERS_H

#include "define.h" // ?

void handleClient(int socket);
int callControlliUser(char *name, char *email, char *password);
int checkUser(char *email, char *password);

#endif
