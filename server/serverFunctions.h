#include "shqcuts.h"

#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#define MAX_CLIENTS 5

void sendClientMessage(char *buffer, int clientfd);
void loginUserRequest(char *name, int clientfd);
void registerUserRequest(char *name, char *pass, int clientfd);
void addClient(char *username, int clientfd);
void handleCommand(unsigned char *buffer, int clientfd);

char  messageToSend[MSG_LEN];

typedef struct {
    int clientfd;
    char username[NAME_LEN];
} Client;

Client clients[MAX_CLIENTS];  

#endif
