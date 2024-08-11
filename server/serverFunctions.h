#include "shqcuts.h"

#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#define MAX_CLIENTS 5

void sendFriendMessage(char *name, char *recipientName, char *message);
void removeFriend(char *name, char *recipientName);
void acceptFriendRequest(char *name, char *recipientName);
void sendFriendRequest(char *name, char *recipientName);
void removeFriendRequest(char *name, char *recipientName);
void createGroup(char *name, char *nameOfGroup);
void loginUserRequest(char *name);
void registerUserRequest(char *name, char *pass);
void addClient(char *username, int clientfd);
void handleCommand(unsigned char *buffer, int clientfd);
void sendClientMessage(char *buffer, char *name);

char  messageToSend[MSG_LEN];

typedef struct {
    int clientfd;
    char username[NAME_LEN];
} Client;

Client clients[MAX_CLIENTS];  

#endif
