#include "shqcuts.h"

#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#define MAX_CLIENTS 5

void addClient(char *username, int clientfd);
void handleCommand(unsigned char *buffer, int clientfd);

typedef struct {
    int clientfd;
    char username[NAME_LEN];
} Client;

Client clients[MAX_CLIENTS];  

#endif
