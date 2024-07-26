#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 256

void removeFReq(char *name);
void clearArrays();

char friendList[MAX_FRIENDS][NAME_LEN];
int numberOfFriends = 0;
int numOfFRequests = 0;
char friendRequests[MAX_FRIENDS][NAME_LEN];

char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN], nameOfFriend[NAME_LEN];

char line[MAX_LINE_LENGTH];
char headers[MAX_USERS][NAME_LEN] = {0};
char friendsArray[MAX_FRIENDS][MAX_USERS][NAME_LEN] = {{{0}}};
int userCount = 0;
int numLines = 0;

int updateFriendFile(char *filename, char *username) {
	FILE *file;
    if ((file = openFile(filename, "r")) == NULL) return -1;	

	clearArrays();

	if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        while (token) {
            strcpy(headers[userCount++], token);
            token = strtok(NULL, ",\n");
        }
    }

	while (fgets(line, sizeof(line), file)) {
        char *linePtr = line;
        char *token = strsep(&linePtr, ",\n");
        int j = 0;

        while (token) {
            strcpy(friendsArray[numLines][j], token);
            j++;
            token = strsep(&linePtr, ",\n");
        }

        numLines++;
    }

    fclose(file);

	int userIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], username) == 0) {
            userIndex = i;
			return userIndex;
        }
    }

    if (userIndex == -1) {
        printf("User %s not found.\n", myName);
        return -1;
    }

	return 0;
}

void clearArrays() {
	userCount = 0;
	numLines = 0;

	line[0] = '\0';

	for (int i = 0; i < MAX_USERS; i++) {
		headers[userCount++][0] = '\0';
	}

	for (int i = 0; i < userCount; i++) {
		for (int j = 0; j < MAX_FRIENDS; j++) {
			friendsArray[j][i][0] = '\0';
		}
	}

	userCount = 0;
}

void initializeFriendFiles(char *filename, const char *username) {	
	FILE *file;
	updateFriendFile(filename, myName);

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
	int userIndex = updateFriendFile(filename, username);

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

void updateFReq(char *name) {
    memset(*friendRequests, 0, sizeof(friendRequests));

	int myIndex = updateFriendFile("friendReq.csv", name);

    int j = 0;
    for (int i = 0; i < numLines; i++) {
		printf("friend Name = %s, friendsArray[%d][%d]\n", friendsArray[i][myIndex], i, myIndex);
        if (strlen(friendsArray[i][myIndex]) > 0) {
            strncpy(friendRequests[i], friendsArray[i][myIndex], NAME_LEN);
			printf("Copying %s into friendRequest[%d]\n", friendsArray[i][myIndex], i);
			j++;
        }
    }
	numOfFRequests = j;
	printf("number of friend requests = %d.\n\n", numOfFRequests);
}

void acceptAllFReq() {
	int userIndex = updateFriendFile("friendReq.csv", myName);

	for (int i = 0; i < MAX_FRIENDS; i++) {
		writeFriendToFile("friends.csv", friendsArray[i][userIndex], myName);
		writeFriendToFile("friends.csv", myName, friendsArray[i][userIndex]);
		removeFReq(friendsArray[i][userIndex]);
		updateFriendArray(friendsArray[i][userIndex]);
	}

	updateFriendArray(myName);
}

void acceptFReq(char *name) {
	writeFriendToFile("friends.csv", myName, name);
	writeFriendToFile("friends.csv", name, myName);
	removeFReq(name);
	updateFriendArray(myName);
	updateFriendArray(name);
}

void removeFReq(char *name) {
	int userIndex = updateFriendFile("friendReq.csv", myName);

    for (int i = 0; i < MAX_FRIENDS; i++) {
        if (strcmp(friendsArray[i][userIndex], name) == 0) {
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

    remove("friendReq.csv");
    rename("tempFReq.csv", "friendReq.csv");
}

void removeAllFReq() {
	int userIndex = updateFriendFile("friends.csv", myName);

	for (int i = 0; i < MAX_FRIENDS; i++) {
		removeFReq(friendsArray[i][userIndex]);
    }
}

void updateFriendArray(char *name) {
    memset(*friendList, 0, sizeof(friendList));
	numberOfFriends = 0;

    int myIndex = updateFriendFile("friends.csv", name);

    for (int i = 0; i < numLines; i++) {
        if (strlen(friendsArray[i][myIndex]) > 0) {
            strncpy(friendList[numberOfFriends++], friendsArray[i][myIndex], NAME_LEN);
        }
    }
}
