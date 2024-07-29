#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#define MAX_CLIENTS 5

void addClient(char *buffer, int clientfd);
void handleCommand(char *buffer, int clientfd);

typedef struct {
    int clientfd;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];  

#endif
