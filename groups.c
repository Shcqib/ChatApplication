#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

#define MAX_GROUPS 2
#define NAME_LEN 20
#define MAX_MEMBERS 3

typedef struct {
    char groupName[NAME_LEN];
    char members[MAX_MEMBERS][NAME_LEN];
    int numberOfMembers;
	
} Group;

Group groups[MAX_GROUPS];
int numOfMembers = 0;
int numberOfGroups = 0;

int groupMenu(Group *currentGroup);
void createAGroup(void);
int seeYourCurrentGroups(void);

int groupsMenu(void) {
	while (1) {
		printf("1) Create a group\n");
		printf("2) Your current groups\n");
		printf("3) Go back\n");
		promptUserForInput("%d", &choice);

		switch (choice) {
            case 1:
                printf("Create a group\n\n");
				createAGroup();
                break;
            case 2:
                seeYourCurrentGroups();
				break;
			case 3:
				menu();
				break;
            default:
                printf("Invalid choice. Please select a valid option.\n\n");
                break;
        }
	}
}

void kickFromGroup(Group *currentGroup) {
	printf("What is the name of the person you would like to kick from the group\n");
	promptUserForInput("%s", nameOfFriend);

	int found = 0;
	int i,j;
	for (i = 0; i < currentGroup->numberOfMembers; i++) {
		if (strcmp(nameOfFriend, currentGroup->members[i]) == 0) {
			if (strcmp(nameOfFriend, myName) == 0) {
				printf("You cannot remove your self.\n\n");
				return;
			}
			found = 1;
			printf("%s kicked from the group.\n\n", nameOfFriend);
			for (j = i; j < (currentGroup->numberOfMembers - 1); j++) {
				strcpy(currentGroup->members[j], currentGroup->members[j + 1]);
			}
			currentGroup->numberOfMembers--;
		}
	}

	if (!found) {
		printf("%s is not in the group %s\n\n", nameOfFriend, currentGroup->groupName);
	}
}

int seeYourCurrentGroups(void) {
	while (1) {
		int i;
		if (numberOfGroups > 0) {
    		printf("Your current groups\n\n");
			for (i = 0; i < numberOfGroups; i++) {
     			printf("%d) %s\n", i + 1, groups[i].groupName);
        	}
			promptUserForInput("%d", &choice);

			groupMenu(&groups[choice - 1]);
        } else {
       		printf("You are in no groups\n\n");
			return 0;
     	}
	}

	return 0;
}

void addGroup() {
	if (numberOfGroups < MAX_GROUPS) {
		char nameOfGroup[NAME_LEN];
   		printf("What would you like to call your group?");
    	promptUserForInput("%s", nameOfGroup);
		if (isValidName(nameOfGroup)) {
			numOfMembers = ++groups[numberOfGroups].numberOfMembers;
   			strcpy(groups[numberOfGroups].groupName, nameOfGroup);
			strcpy(groups[numberOfGroups].members[numOfMembers - 1], myName);
			numberOfGroups++;
			printf("You have successfully created the group %s.\n\n", nameOfGroup);
			groupMenu(&groups[numberOfGroups - 1]);
		} else {
			printf("Invalid name.\n\n");
		}
	} else { 
		 printf("You cant join anymore groups.\n\n");
	}
}

void addToGroup(Group *currentGroup) {
	if (currentGroup->numberOfMembers < MAX_MEMBERS) {
    	printf("Type the name of the person you would like to add to the group.\n\n");
        promptUserForInput("%s", nameOfFriend);
        numOfMembers = ++currentGroup->numberOfMembers;
        strcpy(currentGroup->members[numOfMembers - 1], nameOfFriend);
        printf("Added %s to the group.\n\n", nameOfFriend);
		groupMenu(currentGroup);
    } else {
        printf("Group is full.\n\n");
		return;
    }
}

void leaveGroup(Group *currentGroup) {
	int i, j;

	for (i = 0; i < numberOfGroups; i++) {
		if (strcmp(currentGroup->groupName, groups[i].groupName) == 0) {
			for (j = i; j < numberOfGroups - 1; j++) {
				groups[j] = groups[j + 1]; 
			}
			memset(&groups[j], 0, sizeof(Group));
			numberOfGroups--;
			printf("You have left the group.\n\n");
		}
	}		
}

int groupMenu(Group *currentGroup) {
	while (1) {	
		printf("1) Add to the group\n");
		printf("2) Leave the group\n");
		printf("3) Kick from the group\n");
	    printf("4) See group members\n");
		printf("5) Go back\n");
		promptUserForInput("%d", &choice);

	 	switch (choice) {
       		case 1:
         		addToGroup(currentGroup);
				break;
			case 2:
				leaveGroup(currentGroup);
				groupsMenu();
				break;
			case 3:
				kickFromGroup(currentGroup);
				break;  
         	case 4:
            	printf("Current group members\n\n");
				for	(int i = 0; i < currentGroup->numberOfMembers; i++) {
					printf("%d) %s\n", i + 1, currentGroup->members[i]);	
				}
				printf("\n");
            	break;
			case 5:
				printf("Going back to group menu...\n\n");
				groupsMenu();
         	default:
            	printf("Invalid choice. Please select a valid option.\n\n");
            	break;
		}
	}

	return 0;
}

void createAGroup(void) {
	addGroup();
}
