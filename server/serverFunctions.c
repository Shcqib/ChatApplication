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
	printf("adding client\n");
	bool clientAdded = false;

	for (int i = 0; i < MAX_CLIENTS; i++) {
		printf("first for loop\n");
		if (strcmp(clients[i].username, username) == 0) {
			return;
		}
	}

	for (int i = 0; i < MAX_CLIENTS; i++) {
		printf("Before accessing clients array.\n");
		if (!clientAdded && (strlen(clients[i].username)) == 0) {
			printf("before copying data into clients array\n");
			strcpy(clients[i].username, username);
			clients[i].clientfd = clientfd;
			clientAdded = true;
			printf("Before calling all clients func\n");
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

void writeFReqToFile(char *username, char *friendName) {
	printf("User = %s.\nFriend = %s.\n", username, friendName);
    FILE *file;
    if ((file = openFile("friendReq.csv", "r+")) == NULL) return;

    char line[MAX_LINE_LENGTH];
    char headers[MAX_USERS][NAME_LEN] = {0};
    char friends[MAX_FRIENDS][MAX_USERS][NAME_LEN] = {{{0}}};
    int userCount = 0;
    int numLines = 0;

	char tempName[NAME_LEN];
    if (fgets(line, sizeof(line), file)) {
		printf("Read line for headers: %s\n", line);
        char *token = strtok(line, ",\n");
        while (token) {
			strcpy(tempName, token);
            strncpy(headers[userCount++], tempName, NAME_LEN);
			printf("Copying %s into header[%d].\n", tempName, userCount - 1);
            token = strtok(NULL, ",\n");
        }
    }


    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        int j = 0;
        while (token) {
            if (strlen(token) > 0) {
                strncpy(friends[numLines][j], token, NAME_LEN);
			}
			j++;
            token = strtok(NULL, ",\n");
        }
        numLines++;
    }

	fclose(file);

    int userIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], username) == 0) {
            userIndex = i;
            break;
        }
    }

    if (userIndex == -1) {
        printf("User %s not found.\n", username);
        return;
    }

    for (int i = 0; i < MAX_FRIENDS; i++) {
        if (strlen(friends[i][userIndex]) == 0) {
            strncpy(friends[i][userIndex], friendName, NAME_LEN);
			printf("Copying %s into friends[%d][%d]\n", friendName, i, userIndex);
            break;
        }
    }

    if ((file = openFile("friendReq.csv", "w")) == NULL) return;

	for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

	for (int i = 0; i < MAX_FRIENDS; i++) {
        bool isEmptyLine = true;
        for (int j = 0; j < userCount; j++) {
            if (strlen(friends[i][j]) > 0) {
                isEmptyLine = false;
                break;
            }
        }

        if (!isEmptyLine) {
            for (int j = 0; j < userCount; j++) {
                fprintf(file, "%s", friends[i][j]);
                if (j < userCount - 1) fprintf(file, ",");
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void sendClientMessage(char *buffer, int clientfd) {
    int bytes_sent = send(clientfd, buffer, strlen(buffer) + 1, 0);
	if (bytes_sent <= 0) {
		return;
	}
}

void sendFriendRequest(char *buffer, int clientfd) {
	getRecipientName(buffer, recipientName); 
	getName(buffer, name);

	for (int i = 0; i < MAX_CLIENTS; i++) {                                                                                  
		if (strcmp(recipientName, clients[i].username) == 0) {                                                              
			snprintf(messageToSend, sizeof(messageToSend), "\n%s has sent you a friend request.\n", name);
			sendClientMessage(messageToSend, clients[i].clientfd);
			writeFReqToFile(recipientName, name);
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

    if (type == Connect && dataSize != sizeof(S)) {
        printf("Error: Invalid data size for Connect message\n");
        return;
    }

    void *data = malloc(dataSize);
    if (data == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }

	memcpy(data, buffer + 2, dataSize);

    switch (type) {
		case Connect: {
			S *s = (S *)data;
			printf("Name to send = %s", s->SenderName);
			addClient(s->SenderName, clientfd);
			break;
		}
        case SendFriendRequest: {
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
