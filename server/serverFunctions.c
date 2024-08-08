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

void sendClientMessage(char *buffer, char *name) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (strcmp(name, clients[i].username) == 0) {
			send(clients[i].clientfd, buffer, strlen(buffer) + 1, 0);
		} 
	}
}

void clientDisconnection(char *name) {
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
			sendFriendRequest(sr->SenderName, sr->ReceiverName);	
            break;
        }
		case AddFriendRequest: {
			SR *sr = (SR *)data;
			acceptFriendRequest(sr->SenderName, sr->ReceiverName);	
			break;
		}
		case RemoveFriendRequest: {
			SR *sr = (SR *)data;
			removeFriendRequest(sr->SenderName, sr->ReceiverName);	
			break;
		}
		case RegisterUserRequest: {
			SP *sp = (SP *)data;
			addClient(sp->SenderName, clientfd);
			registerUserRequest(sp->SenderName, sp->SenderPass);
			break;
		}
		case LoginUserRequest: {
			S *s = (S *)data;
			addClient(s->SenderName, clientfd);
			loginUserRequest(s->SenderName);
			break;
		}
		case ClientDisconnect: {
			S *s = (S *)data;
			clientDisconnection(s->SenderName);
			break;
		}
		case ReplaceUsernameRequest: {
			ReplaceUsernameData *data_ = (ReplaceUsernameData *)data;
			replaceUsername(data_->SenderName, data_->PreviousName);
			break;
		}
		case ReplacePasswordRequest: {
			SP *sp = (SP *)data;
			replacePassword(sp->SenderName, sp->SenderPass);
			break;
		}
		case RemoveFriend: {
			SR *sr = (SR *)data;
			removeFriend(sr->SenderName, sr->ReceiverName);
			break;
		}
		case DeactivateAccountRequest: {
			S *s = (S *)data;
			removeUser(s->SenderName);
			break;
		}
        case SendMessageRequest: {
			SRM *srm = (SRM *)data;
			printf("MESSAGE = %s\n", srm->Message);
			sendFriendMessage(srm->SenderName, srm->ReceiverName, srm->Message);
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

