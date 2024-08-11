#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"
#include "serverFunctions.h"

void createGroup(char *name, char *nameOfGroup) {
	writeGroupToFile(nameOfGroup, name);
	writeGroupMemberToFile(nameOfGroup, name);

	snprintf(messageToSend, sizeof(messageToSend), "You have successfully created the group %s.", nameOfGroup);
	sendClientMessage(messageToSend, name);
}

void writeGroupMemberToFile(char *groupName, char *name) {
    char filename[256];

    FILE *file;
    snprintf(filename, sizeof(filename), "data/%s_members.csv", groupName);
    printf("creating file with name %s\n", groupName);
    if ((file = openFile(filename, "a+")) == NULL) return;

    fprintf(file, "%s\n", name);
    fclose(file);
}

void writeGroupToFile(char *groupName, char *name) {
    int userIndex = updateGroupFile(name);

    for (int i = 0; i < MAX_FRIENDS; i++) {
        if (strlen(groupsArray[i][userIndex]) == 0) {
            strcpy(groupsArray[i][userIndex], groupName);
            break;
        }
    }

    FILE *file;
    if ((file = openFile(groupFilePath, "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    fprintf(file, "\n");

    for (int i = 0; i < MAX_FRIENDS; i++) {
        bool isEmptyLine = true;
        for (int j = 0; j < userCount; j++) {
            if (strlen(groupsArray[i][j]) > 0) {
                isEmptyLine = false;
                break;
            }
        }

        if (!isEmptyLine) {
            for (int j = 0; j < userCount; j++) {
                fprintf(file, "%s", groupsArray[i][j]);
                if (j < userCount - 1) fprintf(file, ",");
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}
