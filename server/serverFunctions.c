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

char name[NAME_LEN], recipientName[NAME_LEN], messageToSend[256];
void getName(char *buffer, char *name);
void sendClientMessageThroughSocket(int clientfd, int recipientClient);
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

FILE *openFile(char *filename, char *format) {
    FILE *file = fopen(filename, format);
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    return file;
}

void sendClientMessage(char *buffer, int clientfd) {
    int bytes_sent = send(clientfd, buffer, strlen(buffer) + 1, 0);
	if (bytes_sent <= 0) {
		return;
	}
}

void sendFriendRequest(char *namet, char *recipientNamet, char *message, int clientfd) {
	for (int i = 0; i < MAX_CLIENTS; i++) {                                                                                  
		if (strcmp(recipientNamet, clients[i].username) == 0) {                                                              
			snprintf(messageToSend, sizeof(messageToSend), "You have sent %s a friend request.", recipientNamet);
			sendClientMessage(messageToSend, clientfd);
			sendClientMessage(message, clients[i].clientfd);
			writeFriendToFile("friendReq.csv", recipientNamet, namet);
		}                                                                                                                     
    }  
}

void sendAcceptMsg(char *buffer, int clientfd) {
	getRecipientName(buffer, recipientName);
	getName(buffer, name);

	for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(recipientName, clients[i].username) == 0) {
			snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.\n\n", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
        }
    }

	snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.\n\n", recipientName);
	sendClientMessage(messageToSend, clientfd);
}


void sendDeclineMsg(char *buffer, int clientfd) {
	getRecipientName(buffer, recipientName);
	getName(buffer, name);

	for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(recipientName, clients[i].username) == 0) {
			snprintf(messageToSend, sizeof(messageToSend), "%s declined your friend request.\n\n", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
        }
    }

	snprintf(messageToSend, sizeof(messageToSend), "You declined %s friend request.\n\n", recipientName);
	sendClientMessage(messageToSend, clientfd);
}

void sendFriendMessage(char *buffer, int clientfd) {
	getRecipientName(buffer, recipientName);
    getName(buffer, name);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(recipientName, clients[i].username) == 0) {
            snprintf(messageToSend, sizeof(messageToSend), "%s -> You) \n\n", name);
            sendClientMessage(messageToSend, clients[i].clientfd);
        }
    }

    snprintf(messageToSend, sizeof(messageToSend), "Me -> %s\n\n", recipientName);
    sendClientMessage(messageToSend, clientfd);
}

void deserializeMessage(unsigned char *buffer, int clientfd) {
    MessageType type = buffer[0];
	size_t dataSize = (size_t)buffer[1];
    void *data = malloc(dataSize);
	memcpy(data, buffer + 2, dataSize);

    switch (type) {
		case Connect: {
			S *s = (S *)data;
			addClient(s->SenderName, clientfd);
			break;
		}
        case SendFriendRequest: {
			SRM *srm = (SRM *)data;
			sendFriendRequest(srm->SenderName, srm->ReceiverName, srm->Message, clientfd);	
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

   // if (strcmp(firstWord, "CONNECT") == 0) {
  //      addClient(buffer, clientfd);
//	} else if (strcmp(firstWord, "FRIENDREQUEST") == 0) {
//		sendFriendRequest(buffer, clientfd);	
//	} else if (strcmp(firstWord, "DECLINEFRIENDREQ") == 0) {
//		sendDeclineMsg(buffer, clientfd);
//	} else if (strcmp(firstWord, "ACCEPTFRIENDREQ") == 0) {
//		sendAcceptMsg(buffer, clientfd);
//	} else if (strcmp(firstWord, "MESSAGE") == 0 ) {
//		sendFriendMessage(buffer, clientfd);
//	}

void getRecipientName(char *buffer, char *name) {
	char *firstSpacePosition, *secondSpacePosition, *thirdSpacePosition;

    firstSpacePosition = strchr(buffer, ' ');
    if (firstSpacePosition == NULL) {
        return; 
    }

    char *secondWordStart = firstSpacePosition + 1;
    secondSpacePosition = strchr(secondWordStart, ' ');
    if (secondSpacePosition == NULL) {
        return; 
    }

    char *thirdWordStart = secondSpacePosition + 1;
    thirdSpacePosition = strchr(thirdWordStart, ' ');

    if (thirdSpacePosition != NULL) {
        int lengthToCopy = thirdSpacePosition - thirdWordStart;
        strncpy(name, thirdWordStart, lengthToCopy);
        name[lengthToCopy] = '\0';
    } else {
        strcpy(name, thirdWordStart);
    }
}

void getName(char *buffer, char *name) {
    char *firstSpacePosition, *secondSpacePosition;

    firstSpacePosition = strchr(buffer, ' ');

    if (firstSpacePosition != NULL) {
        char *secondWordStart = firstSpacePosition + 1;

        secondSpacePosition = strchr(secondWordStart, ' ');

        if (secondSpacePosition != NULL) {
            int lengthToCopy = secondSpacePosition - secondWordStart;
            strncpy(name, secondWordStart, lengthToCopy);
            name[lengthToCopy] = '\0';
        } else {
            strcpy(name, secondWordStart);
        }
	}
}

void getExtraInfo(char *buffer, char *message) {
	char *firstSpacePosition, *secondSpacePosition, *thirdSpacePosition, *fourthWordStart;

    firstSpacePosition = strchr(buffer, ' ');

    if (firstSpacePosition != NULL) {
        char *secondWordStart = firstSpacePosition + 1;
        secondSpacePosition = strchr(secondWordStart, ' ');

        if (secondSpacePosition != NULL) {
            char *thirdWordStart = secondSpacePosition + 1;
            thirdSpacePosition = strchr(thirdWordStart, ' ');

            if (thirdSpacePosition != NULL) {
                fourthWordStart = thirdSpacePosition + 1;
                strcpy(message, fourthWordStart);
            }
		}
	}
}
