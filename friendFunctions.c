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

char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN], nameOfFriend[NAME_LEN];

void initializeFriendFiles(char *filename, const char *username) {
    FILE *file;
    if ((file = openFile(filename, "r+")) == NULL) return;

    char line[MAX_LINE_LENGTH];
    char headers[MAX_USERS][NAME_LEN];
    char friends[MAX_FRIENDS][MAX_USERS][NAME_LEN];
    int userCount = 0;
    int numLines = 0;

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
                strncpy(friends[numLines][j], token, MAX_LINE_LENGTH);
                j++;
            }
            token = strtok(NULL, ",\n");
        }
        numLines++;
    }

    fclose(file);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], username) == 0) {
            printf("User %s already exists in the friends file.\n", username);
            return;
        }
    }
	
	strncpy(headers[userCount++], username, MAX_LINE_LENGTH);

    if ((file = openFile(filename, "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        for (int j = 0; j < userCount; j++) {
            fprintf(file, "%s", friends[i][j]);
            if (j < userCount - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void writeFriendToFile(char *username, char *friendName) {
    FILE *file;
    if ((file = openFile("friends.csv", "r+")) == NULL) return;

    char line[MAX_LINE_LENGTH];
    char headers[MAX_USERS][NAME_LEN];
    char friends[MAX_FRIENDS][MAX_USERS][NAME_LEN];
    int userCount = 0;
    int numLines = 0;
    int friendCounts[MAX_USERS] = {0};

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
                strncpy(friends[numLines][j], token, MAX_LINE_LENGTH);
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
        if (strlen(friends[i][userIndex]) == 0) {
            strncpy(friends[i][userIndex], friendName, MAX_LINE_LENGTH);
            break;
        }
    }

	if ((file = openFile("friends.csv", "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        for (int j = 0; j < userCount; j++) {
            fprintf(file, "%s", friends[i][j]);
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

    char line[MAX_LINE_LENGTH];
    char headers[MAX_USERS][NAME_LEN];
    char friends[MAX_FRIENDS][MAX_USERS][NAME_LEN];
    int userCount = 0;
    int numLines = 0;

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
                strncpy(friends[numLines][j], token, NAME_LEN);
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

    numOfFRequests = 0;
    for (int i = 0; i < numLines; i++) {
        if (strlen(friends[i][myIndex]) > 0) {
            strncpy(friendRequests[numOfFRequests++], friends[i][myIndex], NAME_LEN);
        }
    }
}

void acceptAllFReq() {
    FILE *file;
    if ((file = openFile("friendReq.csv", "r")) == NULL) return;

    while (fscanf(file, "%s\n", nameInFile) != EOF) {
        writeFriendToFile(nameInFile, myName);
        declineAllFReq();
        updateFReq();
        break;
    }

    fclose(file);
}

void acceptFReq(char *name) {
     FILE *file;
    if ((file = openFile("friendReq.csv", "r")) == NULL) return;

    while (fscanf(file, "%s\n", nameInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            writeFriendToFile(name, myName);
            declineFReq(name);
            updateFReq();
        }
    }

    fclose(file);
}

void declineFReq(char *name) {
    FILE *file, *tempFile;
    if ((file = openFile("friendReq.csv", "r")) == NULL) return;
    if ((tempFile = openFile("tempFReq.csv", "a")) == NULL) return;

    int found = false;

    while (fscanf(file, "%s\n", nameInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            found = true;
        } else {
            fprintf(tempFile, "%s\n", nameInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove("friendReq.csv");
        rename("tempFReq.csv", "friendReq.csv");
        updateFReq();
    } else {
        remove("tempFReq.csv");
    }
}

void declineAllFReq() {
    remove("friendReq.csv");
    rename("tempFReq.csv", "friendReq.csv");
    updateFReq();
}

void updateFriendArray() {
    FILE *file;
    if ((file = openFile("friends.csv", "r")) == NULL) return;

    char line[MAX_LINE_LENGTH];
    char headers[MAX_USERS][NAME_LEN];
    char friends[MAX_FRIENDS][MAX_USERS][NAME_LEN];
    int userCount = 0;
    int numLines = 0;

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
                strncpy(friends[numLines][j], token, NAME_LEN);
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

    numberOfFriends = 0;
    for (int i = 0; i < numLines; i++) {
        if (strlen(friends[i][myIndex]) > 0) {
            strncpy(friendList[numberOfFriends++], friends[i][myIndex], NAME_LEN);
        }
    }
}
void addFriendToArray(char *username, char *friendName) {
    writeFriendToFile(username, friendName);
    updateFriendArray();
}

