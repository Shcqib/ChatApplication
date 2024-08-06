#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "serverFunctions.h"
#include "shqcuts.h"
#include <stdbool.h>

char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN];

void registerUserRequest(char *name, char *pass, int clientfd) {
	snprintf(messageToSend , sizeof(messageToSend), "Welcome %s", name);
	sendClientMessage(messageToSend, clientfd);
	active = true;
	writeUserToFile(usersFilePath, name, pass, active);
}

void loginUserRequest(char *name, int clientfd) {
	printf("calling loginUserRequest\n");
	snprintf(messageToSend , sizeof(messageToSend), "Welcome %s", name);
	sendClientMessage(messageToSend, clientfd);
	active = true;
	updateStatus(name, active);
}

void writeUserToFile(char *filename, char *name, char *pass, bool status) {
    FILE *file;
    if ((file = openFile(filename, "a")) == NULL) return;

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fprintf(file, "username,password,status\n");
    }

    fseek(file, 0, SEEK_SET);

    if (status) {
        fprintf(file, "%s,%s,Online\n", name, pass);
    } else {
        fprintf(file, "%s,%s,Offline\n", name, pass);
    }
    fclose(file);

    initializeGroupFile(name);
    initializeFile("data/friends.csv", name);
    initializeFile("data/friendReq.csv", name);
}

void updateStatus(char *name, bool status) {
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
		printf("writing %s to file %s with pass %s\n", name, "tempUsers.csv", pass);
        writeUserToFile("tempUsers.csv", name, pass, status);
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
    } else {
        remove("tempUsers.csv");
    }
}

void replaceUsername(char *name, char *prevName) {
    FILE *file, *tempFile;
    if  ((file = openFile(usersFilePath, "r+")) == NULL) return;
    if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    char pass[NAME_LEN];
    int found = 0;

    fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, prevName) == 0) {
            strcpy(pass, passInFile);
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
		active = true;
        writeUserToFile("tempUsers.csv", name, pass, active);
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
        printf("Username successfully changed.\n\n");
    } else {
        remove("tempUsers.csv");
    }
}

void replacePassword(char *name, char *pass) {
    FILE *file, *tempFile;
    if  ((file = openFile(usersFilePath, "r")) == NULL) return;
    if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    int found = 0;
    fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
		active = true;
        writeUserToFile("tempUsers.csv", name, pass, active);
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
        printf("Password successfully changed.\n\n");
    } else {
        remove("tempUsers.csv");
    }
}
