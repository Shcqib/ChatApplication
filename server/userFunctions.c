#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"
#include <stdbool.h>

void registerUserRequest(char *name, int clientfd) {
	snprintf(messageToSend , sizeof(messageToSend), "Welcome %s", name);
	sendClientMessage(messageToSend, clientfd);
}

void loginUserRequest(char *name, int clientfd) {
	snprintf(messageToSend , sizeof(messageToSend), "Welcome %s", name);
	sendClientMessage(messageToSend, clientfd);
}

void writeUserToFile(char *filename, char *name, char *pass) {
    FILE *file;
    if ((file = openFile(filename, "a")) == NULL) return;

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fprintf(file, "username,password,status\n");
    }

    fseek(file, 0, SEEK_SET);

    if (active) {
        fprintf(file, "%s,%s,Online\n", name, pass);
    } else {
        fprintf(file, "%s,%s,Offline\n", name, pass);
    }
    fclose(file);

    initializeGroupFile(name);
    initializeFile("data/friends.csv", name);
    initializeFile("data/friendReq.csv", name);
}

void updateStatus(char *name) {
    FILE *file, *tempFile;
    if  ((file = openFile(usersFilePath, "r+")) == NULL) return;
    if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    int found = 0;
    char pass[NAME_LEN];

    fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            strcpy(pass, passInFile);
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        writeUserToFile("tempUsers.csv", name, pass);
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
    } else {
        remove("tempUsers.csv");
    }
}
