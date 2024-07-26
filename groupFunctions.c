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

char line[MAX_LINE_LENGTH];
char groupsArray[MAX_GROUPS][MAX_USERS][NAME_LEN] = {{{0}}};

void clearVariables() {
    userCount = 0;
    numLines = 0;

    line[0] = '\0';

    for (int i = 0; i < MAX_USERS; i++) {
        headers[i][0] = '\0';
    }

    for (int i = 0; i < MAX_GROUPS; i++) {
        for (int j = 0; j < MAX_FRIENDS; j++) {
            groupsArray[j][i][0] = '\0';
        }
    }
}

int updateGroupFile(char *username) {
    FILE *file;
    if ((file = openFile("groups.csv", "r")) == NULL) return -1;

	clearVariables();

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
            strcpy(groupsArray[numLines][j], token);
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

void initializeGroupFile(char *username) {
	updateGroupFile(username);

	for (int i = 0; i < userCount; i++) {
        if (strcmp(headers[i], username) == 0) {
            return;
        }
    }

    strcpy(headers[userCount++], username);

	FILE *file;
    if ((file = openFile("groups.csv", "w")) == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s", headers[i]);
        if (i < userCount - 1) fprintf(file, ",");
    }
    printf("\n");

	for (int i = 0; i < numLines; i++) {
		int isEmptyLine = true;
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

void updateGroupArray(char *filename) {
    FILE *file;
    if ((file = openFile(filename, "r")) == NULL) return;

    char groupName[NAME_LEN], name[NAME_LEN], role[NAME_LEN];
    int j = 0;

    if (fscanf(file, "%s\n", groupName) == EOF) {
        fclose(file);
        return;
    }

    int groupIndex = -1;
    for (int i = 0; i < numberOfGroups; i++) {
        if (strcmp(groups[i].groupName, groupName) == 0) {
            groupIndex = i;
            break;
        }
    }

    if (groupIndex == -1) {
        groupIndex = numberOfGroups++;
        strcpy(groups[groupIndex].groupName, groupName);
        groups[groupIndex].amountOfMembers = 0;
    }

    while (fscanf(file, "%[^,],%s\n", role, name) != EOF) {
        strcpy(groups[groupIndex].members[j].name, name);
        strcpy(groups[groupIndex].members[j].role, role);
        j++;
    }

    groups[groupIndex].amountOfMembers = j;
    numOfMembers = j;

    fclose(file);
}
void removeGroup(char *filename, char *groupname) {
    FILE *file, *tempFile;
    if  ((file = openFile("groups.csv", "r+")) == NULL) return;
    if  ((tempFile = openFile("tempGroups.csv", "a")) == NULL) return;
    
    int found = 0;
    char nameOfGroup[NAME_LEN];
    
    while (fscanf(file, "%s\n", nameOfGroup) != EOF) {
        if (strcmp(nameOfGroup, filename) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s\n", nameOfGroup);
        }
    }
    
    fclose(file);
    fclose(tempFile);
            
    if (found) {
        remove("groups.csv"); 
        remove(filename);
        rename("tempGroups.csv", "groups.csv");
        for (int i = 0; i < numberOfGroups; i++) {
            if (strcmp(groups[i].groupName, groupname) == 0) {
                groups[i] = groups[i + 1];
            }
        }
        numberOfGroups--;
    } else {
        remove("tempGroups.csv");
    }
}

void writeGroupMemberToFile(char *filename, char *name, char *role) {
    FILE *file;
    if ((file = openFile(filename, "a")) == NULL) return;

    fprintf(file, "%s,%s\n", role, name);

    fclose(file);
}

void updateGroups() {
    FILE *file;
    if ((file = openFile("groups.csv", "r")) == NULL) return;

    char nameOfGroup[NAME_LEN];

    while (fscanf(file, "%s\n", nameOfGroup) != EOF) {
        updateGroupArray(nameOfGroup);
    }

    fclose(file);
}

void writeToFile(char *filename, char *name) {
    FILE *file, *groupFile;
    if ((file = openFile(filename, "a")) == NULL) return;
    if ((groupFile = openFile("groups.csv", "a")) == NULL) return;

    fprintf(groupFile, "%s\n", filename);
    fprintf(file, "%s\n", name);
    fclose(file);
    fclose(groupFile);
}
