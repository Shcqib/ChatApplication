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

char groupsArray[MAX_GROUPS][MAX_USERS][NAME_LEN] = {{{0}}};
char membersArray[MAX_MEMBERS][MAX_GROUPS][NAME_LEN] = {{{0}}};
int numOfMembers = 0;
int numberOfGroups = 0;

int updateGroupFile(char *username);

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

void updateGroupMemberFile(int userIndex) {
	bool found = false;
	for (int i = 0; i < numLines; i++) {
		char filename[256];
		FILE *file;
		snprintf(filename, sizeof(filename), "data/%s_members.csv", groupsArray[i][userIndex]);
		if ((file = openFile(filename, "r")) == NULL) return;

		while (fgets(line, sizeof(line), file)) {
            char *token = strtok(line, "\n");
            while (token) {
                if (strcmp(token, myName) == 0) {
                    found = true;
                    rewind(file);
                    break;
                }
                token = strtok(NULL, "\n");
            }
            if (found) break;  
        }

        if (found) {
            while (fgets(line, sizeof(line), file)) {
                char *token = strtok(line, "\n");
                while (token) {
					for (int j = 0; j < MAX_MEMBERS; j++) {
						if (strlen(token) > 0) {
							strcpy(membersArray[j][i], token);
						}
					}
					token = strtok(NULL, "\n");
                }
            }
        }
	fclose(file);
	}
}

int updateGroupFile(char *username) {
	printf("updating group file\n");
    FILE *file, memberFile;
    if ((file = openFile(groupFilePath, "r")) == NULL) return -1;

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
			updateGroupMemberFile(userIndex);
            return userIndex;
        }
    }

    if (userIndex == -1) {
        printf("User %s not found.\n", username);
        return -1;
    }

    return 0;
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

void updateGroups() {
	int userIndex = updateGroupFile(myName);

	for (int i = 0; i < MAX_GROUPS; i++) {
		if (strlen(groupsArray[i][userIndex]) > 0) {
			if (strlen(groups[i].groupName) == 0) {
				strcpy(groups[i].groupName, groupsArray[i][userIndex]);
				numberOfGroups++;
				for (int k = 0; k < MAX_MEMBERS; k++) {
					if (strlen(membersArray[k][numLines]) > 0) {
						strcpy(groups[i].members[k].name, membersArray[k][numLines]);	
					}
				}
			}
		}
	}
}
