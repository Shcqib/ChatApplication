#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include "serverFunctions.h"
#include <stdbool.h>
#include "shqcuts.h"

#define MAX_LINE_LENGTH 256

char message[MSG_LEN];
void getName(char *buffer, char *name);
void getRecipientName(char *buffer, char *name);

void allClients() {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		printf("Client %d : Name = %s : FD = %d\n", i + 1, clients[i].username, clients[i].clientfd);
	}
}

void addClient(char *username, int clientfd) {
	bool clientAdded = false;

	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (strcmp(clients[i].username, username) == 0) {
			return;
		}
	}

	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (!clientAdded && (strlen(clients[i].username)) == 0) {
			strcpy(clients[i].username, username);
			clients[i].clientfd = clientfd;
			clientAdded = true;
			allClients();
			return;
		}
	}
}

void sendClientMessage(char *buffer, int clientfd) {
    int bytes_sent = send(clientfd, buffer, strlen(buffer) + 1, 0);
	if (bytes_sent < 0) {
        perror("send failed");
    } else if (bytes_sent == 0) {
        printf("Sending 0 bytes\n");
    } else {
        printf("Sent %d bytes: %s\n", bytes_sent, buffer);
    }
}

void sendFriendRequest(char *name, char *recipientName, int clientfd) {
	for (int i = 0; i < MAX_CLIENTS; i++) {                                                                                  
		if (strcmp(recipientName, clients[i].username) == 0) {                                                              
			snprintf(messageToSend, sizeof(messageToSend), "You have sent %s a friend request.", recipientName);
			sendClientMessage(messageToSend, clientfd);
			snprintf(messageToSend, sizeof(messageToSend), "%s sent you a friend request.", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
			writeFriendToFile("data/friendReq.csv", recipientName, name);
			return;
		}                                                                                                                     
    }  
}

void acceptFriendRequest(char *name, char *recipientName, int clientfd) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(recipientName, clients[i].username) == 0) {
			snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.\n\n", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
			acceptFReq(recipientName, name);
			break;
        }
    }


	snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.\n\n", recipientName);
	sendClientMessage(messageToSend, clientfd);
	acceptFReq(name, recipientName);
}


void removeFriendRequest(char *name, char *recipientName, int clientfd) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(recipientName, clients[i].username) == 0) {
			snprintf(messageToSend, sizeof(messageToSend), "%s declined your friend request.\n\n", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
			break;
        }
    }
			snprintf(messageToSend, sizeof(messageToSend), "You declined %s friend request.\n\n", recipientName);
			sendClientMessage(messageToSend, clientfd);
			removeFReq(name, recipientName);
}

void sendFriendMessage(char *buffer, int clientfd) {
}

void clientDisconnection(char *name, int clientfd) {
	active = false;
	updateStatus(name, active);
}

void deserializeMessage(unsigned char *buffer, int clientfd) {
    MessageType type = buffer[0];
	size_t dataSize = (size_t)buffer[1];
    void *data = malloc(dataSize);
	memcpy(data, buffer + 2, dataSize);

    switch (type) {
        case SendFriendRequest: {
			SR *sr = (SR *)data;
			sendFriendRequest(sr->SenderName, sr->ReceiverName, clientfd);	
            break;
        }
		case AddFriendRequest: {
			SR *sr = (SR *)data;
			acceptFriendRequest(sr->SenderName, sr->ReceiverName, clientfd);	
			break;
		}
		case RemoveFriendRequest: {
			SR *sr = (SR *)data;
			removeFriendRequest(sr->SenderName, sr->ReceiverName, clientfd);	
			break;
		}
		case RegisterUserRequest: {
			SP *sp = (registrationData *)data;
			addClient(sp->SenderName, clientfd);
			registerUserRequest(sp->SenderName, sp->SenderPass, clientfd);
			break;
		}
		case LoginUserRequest: {
			S *s = (S *)data;
			addClient(s->SenderName, clientfd);
			loginUserRequest(s->SenderName, clientfd);
			break;
		}
		case ClientDisconnect: {
			S *s = (S *)data;
			clientDisconnection(s->SenderName, clientfd);
			break;
		}
        case SendMessageRequest: {
            break;
        }
        default:
            printf("Unknown message type\n");
            break;
    }

    free(data);
}

void handleCommand(unsigned char *buffer, int clientfd) {
	deserializeMessage(buffer, clientfd);
}

