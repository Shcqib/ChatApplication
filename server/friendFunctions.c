#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"
#include "serverFunctions.h"
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void removeFriendFromFile(char *name, char *recipientName);
void removeFReq(char *name, char *friendName);
void clearArrays();

char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN], nameOfFriend[NAME_LEN];

void initializeFile(char *filename, const char *username) {	
	FILE *file;
	updateFile(filename, myName);

	for (int i = 0; i < userCount; i++) {
		if (strcmp(headers[i], username) == 0) {
			return;
		}
	}

	strncpy(headers[userCount++], username, NAME_LEN);

    if ((file = openFile(filename, "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
	printf("\n");

	for (int i = 0; i < numLines; i++) {
        int isEmptyLine = true;
        for (int j = 0; j < userCount; j++) {
            if (strlen(friendsArray[i][j]) > 0) {
                isEmptyLine = false;
                break;
            }
        }
        if (!isEmptyLine) {
            for (int j = 0; j < userCount; j++) {
                fprintf(file, "%s", friendsArray[i][j]);
                if (j < userCount - 1) fprintf(file, ",");
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void writeFriendToFile(char *filename, char *username, char *friendName) {
	FILE *file;
	int userIndex = updateFile(filename, username);

    for (int i = 0; i < MAX_FRIENDS; i++) {
        if (strlen(friendsArray[i][userIndex]) == 0) {
            strncpy(friendsArray[i][userIndex], friendName, MAX_LINE_LENGTH);
            break;
        }
    }

	if ((file = openFile(filename, "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        bool isEmptyLine = true;
        for (int j = 0; j < userCount; j++) {
            if (strlen(friendsArray[i][j]) > 0) {
                isEmptyLine = false;
                break;
            }
        }

        if (!isEmptyLine) {
            for (int j = 0; j < userCount; j++) {
                fprintf(file, "%s", friendsArray[i][j]);
                if (j < userCount - 1) fprintf(file, ",");
            }
            fprintf(file, "\n");
        }
    }

	fclose(file);
}

void removeFriend(char *name, char *recipientName) {
	snprintf(messageToSend, sizeof(messageToSend), "You removed %s as a friend.", recipientName);
	sendClientMessage(messageToSend, name);
	removeFriendFromFile(recipientName, name);

	snprintf(messageToSend, sizeof(messageToSend), "%s removed you as a friend.", name);
	sendClientMessage(messageToSend, recipientName);
	removeFriendFromFile(name, recipientName);
}

void removeFriendFromFile(char *name, char *recipientName) {
	int userIndex = updateFile(friendsFilePath, name);

	for (int i = 0; i < MAX_FRIENDS; i++) {
		if (strcmp(recipientName, friendsArray[i][userIndex]) == 0) {
			friendsArray[i][userIndex][0] = '\0';
		}
    }

	FILE *file;
    if ((file = openFile(friendsFilePath, "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        bool isEmptyLine = true;
        for (int j = 0; j < userCount; j++) {
            if (strlen(friendsArray[i][j]) > 0) {
                isEmptyLine = false;
                break;
            }
        }

        if (!isEmptyLine) {
            for (int j = 0; j < userCount; j++) {
                fprintf(file, "%s", friendsArray[i][j]);
                if (j < userCount - 1) fprintf(file, ",");
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void acceptAllFReq() {
	int userIndex = updateFile(friendReqFilePath, myName);

	for (int i = 0; i < MAX_FRIENDS; i++) {
		writeFriendToFile(friendsFilePath, friendsArray[i][userIndex], myName);
		writeFriendToFile(friendsFilePath, myName, friendsArray[i][userIndex]);
		removeFReq(friendsArray[i][userIndex], "p");
	}

	updateFriendArray(myName);
}

void acceptFReq(char *name, char *friendName) {
	printf("adding %s as a friend of %s\n", friendName, name);
	writeFriendToFile(friendsFilePath, name, friendName);
	removeFReq(name, friendName);
	updateFriendArray(name);
}

void removeFReq(char *name, char *friendName) {
	int userIndex = updateFile(friendReqFilePath, name);

    for (int i = 0; i < MAX_FRIENDS; i++) {
        if (strcmp(friendsArray[i][userIndex], friendName) == 0) {
            friendsArray[i][userIndex][0] = '\0';
        }
    }

    FILE *file;
    if ((file = openFile("tempFReq.csv", "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        for (int j = 0; j < userCount; j++) {
			if (strlen(friendsArray[i][j]) > 0) {
				fprintf(file, "%s", friendsArray[i][j]);
			}
            if (j < userCount - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fclose(file);

    remove(friendReqFilePath);
    rename("tempFReq.csv", friendReqFilePath);
}

void removeAllFReq() {
	int userIndex = updateFile(friendsFilePath, myName);

	for (int i = 0; i < MAX_FRIENDS; i++) {
		removeFReq("p", friendsArray[i][userIndex]);
    }
}

void sendFriendRequest(char *name, char *recipientName) {
	snprintf(messageToSend, sizeof(messageToSend), "You have sent %s a friend request.", recipientName);
	sendClientMessage(messageToSend, name);

	snprintf(messageToSend, sizeof(messageToSend), "%s sent you a friend request.", name);
	sendClientMessage(messageToSend, recipientName);
	writeFriendToFile(friendReqFilePath, recipientName, name);
}

void acceptFriendRequest(char *name, char *recipientName) {
	snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.", name);
	sendClientMessage(messageToSend, recipientName);
	acceptFReq(recipientName, name);

    snprintf(messageToSend, sizeof(messageToSend), "You and %s are now friends.", recipientName);
    sendClientMessage(messageToSend, name);
    acceptFReq(name, recipientName);
}


void removeFriendRequest(char *name, char *recipientName) {
	snprintf(messageToSend, sizeof(messageToSend), "%s declined your friend request.", name);
	sendClientMessage(messageToSend, recipientName);

	snprintf(messageToSend, sizeof(messageToSend), "You declined %s friend request.", recipientName);
	sendClientMessage(messageToSend, name);
	removeFReq(name, recipientName);
}

void sendFriendMessage(char *name, char *recipientName, char *message) {
	snprintf(messageToSend, sizeof(messageToSend), "You -> %s : %s", recipientName, message);
	sendClientMessage(messageToSend, name);

	snprintf(messageToSend, sizeof(messageToSend), "%s -> You : %s", name, message);
	sendClientMessage(messageToSend, recipientName);
}

