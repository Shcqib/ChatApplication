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

char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN], nameOfFriend[NAME_LEN];

char line[MAX_LINE_LENGTH];
char headers[MAX_USERS][NAME_LEN] = {0};
char friendsArray[MAX_FRIENDS][MAX_USERS][NAME_LEN] = {{{0}}};
int userCount = 0;
int numLines = 0;

int updateFriendFile(char *filename) {
	FILE *file;
    if ((file = openFile(filename, "r")) == NULL) return -1;	

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
        if (strcmp(headers[i], myName) == 0) {
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

void initializeFriendFiles(char *filename, const char *username) {
    FILE *file;
    if ((file = openFile(filename, "r+")) == NULL) return;

    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        while (token) {
            strncpy(headers[userCount++], token, NAME_LEN);
            token = strtok(NULL, ",\n");
        }
    }

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        int j = 0;
        while (token) {
            if (strlen(token) > 0) {
                strncpy(friendsArray[numLines][j], token, NAME_LEN);
                j++;
            }
            token = strtok(NULL, ",\n");
        }
        numLines++;
    }

    fclose(file);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], username) == 0) {
            printf("User %s already exists in the friendsArray file.\n", username);
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

void writeFriendToFile(char *username, char *friendName) {
    FILE *file;
    if ((file = openFile("friendsArray.csv", "r+")) == NULL) return;

    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        while (token) {
            strncpy(headers[userCount++], token, MAX_LINE_LENGTH);
            token = strtok(NULL, ",\n");
        }
    }

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        int j = 0;
        while (token) {
            if (strlen(token) > 0) {
                strncpy(friendsArray[numLines][j], token, MAX_LINE_LENGTH);
                j++;
            }
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
        if (strlen(friendsArray[i][userIndex]) == 0) {
            strncpy(friendsArray[i][userIndex], friendName, MAX_LINE_LENGTH);
            break;
        }
    }

	if ((file = openFile("friendsArray.csv", "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        for (int j = 0; j < userCount; j++) {
            fprintf(file, "%s", friendsArray[i][j]);
            if (j < userCount - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void updateFReq() {
    memset(*friendRequests, 0, sizeof(friendRequests));

    FILE *file;
    if ((file = openFile("friendReq.csv", "r")) == NULL) return;

    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        while (token) {
            strcpy(headers[userCount++], token);
			printf("copying %s into headers[%d]\n", token, userCount - 1);
            token = strtok(NULL, ",\n");
        }
    }

	while (fgets(line, sizeof(line), file)) {
		printf("Read line from friendsArray: %s\n", line);

		char *linePtr = line;
		char *token = strsep(&linePtr, ",\n");
		int j = 0;
		
		while (token) {
			strncpy(friendsArray[numLines][j], token, NAME_LEN - 1);
			printf("Copying %s into friendsArray[%d][%d].\n", token, numLines, j);
			j++;
			token = strsep(&linePtr, ",\n");
		}

		numLines++;
	}
    fclose(file);

    int myIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], myName) == 0) {
			printf("%s is equal to header[%d]\n", myName, i);
            myIndex = i;
            break;
        }
    }

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
	int userIndex = updateFriendFile("friends.csv");

	for (int i = 0; i < MAX_FRIENDS; i++) {
		writeFriendToFile(friendsArray[i][userIndex], myName);
		removeFReq(friendsArray[i][userIndex]);
	}
}

void acceptFReq(char *name) {
	writeFriendToFile(name, myName);
	removeFReq(name);
}

void removeFReq(char *name) {
	int userIndex = updateFriendFile("friendReq.csv");

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
            fprintf(file, "%s", friendsArray[i][j]);
            if (j < userCount - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fclose(file);

    remove("friendsArray.csv");
    rename("tempFReq.csv", "friendsArray.csv");
}

void removeAllFReq() {
	int userIndex = updateFriendFile("friends.csv");

	for (int i = 0; i < MAX_FRIENDS; i++) {
		removeFReq(friendsArray[i][userIndex]);
    }
}

void updateFriendArray() {
    FILE *file;
    if ((file = openFile("friendsArray.csv", "r")) == NULL) return;

    if (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        while (token) {
            strncpy(headers[userCount++], token, NAME_LEN);
            token = strtok(NULL, ",\n");
        }
    }

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",\n");
        int j = 0;
        while (token) {
            if (strlen(token) > 0) {
                strncpy(friendsArray[numLines][j], token, NAME_LEN);
                j++;
            }
            token = strtok(NULL, ",\n");
        }
        numLines++;
    }

    fclose(file);

    int myIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], myName) == 0) {
            myIndex = i;
            break;
        }
    }

    int i = 0;
    for (i = 0; i < numLines; i++) {
        if (strlen(friendsArray[i][myIndex]) > 0) {
            strncpy(friendList[i], friendsArray[i][myIndex], NAME_LEN);
        }
    }
	numberOfFriends = i;
}
void addFriendToArray(char *username, char *friendName) {
    writeFriendToFile(username, friendName);
    updateFriendArray();
}
