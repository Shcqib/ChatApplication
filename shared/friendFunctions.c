#include <stdio.h>                                                                                        
#include <stdlib.h>                                                                                       
#include <ctype.h>                                                                                        
#include <string.h>                                                                                       
#include "shqcuts.h"                                                                                      
#include <stdbool.h>                                                                                      

void clearArrays() {
    userCount = 0;
    numLines = 0;

    line[0] = '\0';

    for (int i = 0; i < MAX_USERS; i++) {
        headers[i][0] = '\0';
    }

    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < MAX_FRIENDS; j++) {
            friendsArray[j][i][0] = '\0';
        }
    }
}

void updateFReq(char *name) {
    memset(*friendRequests, 0, sizeof(friendRequests));

    int myIndex = updateFile(friendReqFilePath, name);

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

void updateFriendArray(char *name) {
    memset(*friendList, 0, sizeof(friendList));
    numberOfFriends = 0;

    int myIndex = updateFile(friendsFilePath, name);

    for (int i = 0; i < numLines; i++) {
        if (strlen(friendsArray[i][myIndex]) > 0) {
            strncpy(friendList[numberOfFriends++], friendsArray[i][myIndex], NAME_LEN);
        }
    }
}

int updateFile(char *filename, char *username) {
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
        printf("User %s not found.\n", username);
        return -1;
    }

    return 0;
}
