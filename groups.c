#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"
#include <stdbool.h>

int numOfMembers = 0;
int numberOfGroups = 0;

int groupMenu(Group *currentGroup);
void createGroup(void);
void groupMemberList(Group *currentGroup);
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
				createGroup();
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

void kickFromGroup(Member *member, Group *currentGroup) {
	int j, i;

	if (strcmp(member->role, "Admin") == 0) {
		printf("You cannot remove an admin.\n\n");
			return;
	}

	for (i = 0; i < currentGroup->amountOfMembers; i++) {
		if (strcmp(member->name, currentGroup->members[i].name) == 0) {	
			char filename[NAME_LEN];
			snprintf(filename, sizeof(filename), "%sMembers.csv", currentGroup->groupName);
			removeMember(member->name, filename);
			updateGroups();
			printf("%s kicked from the group.\n\n", member->name);
			groupMemberList(currentGroup);
		}
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

void createGroup() {
	if (numberOfGroups < MAX_GROUPS) {
		char nameOfGroup[NAME_LEN];
   		printf("What would you like to call your group?\n");
		if (isValidName(nameOfGroup)) {
			writeGroupToFile(nameOfGroup);
			writeGroupMemberToFile(nameOfGroup, myName);
			updateGroups();
			printf("You have successfully created the group %s.\n\n", nameOfGroup);
			groupMenu(&groups[numberOfGroups - 1]);
		}
	} else { 
		 printf("You cant join anymore groups.\n\n");
	}
}

void addToGroup(Group *currentGroup) {
	if (currentGroup->amountOfMembers < MAX_MEMBERS) {
    	printf("Type the name of the person you would like to add to the group.\n");
		if (isValidName(nameOfFriend)) {
			if (strcmp(nameOfFriend, myName) == 0) {
				printf("You are already in this group.\n\n");
				return;
			}

			for (int i = 0; i < currentGroup->amountOfMembers; i++) {
				if (strcmp(nameOfFriend, currentGroup->members[i].name) == 0) {
					printf("This person is already in the group.\n\n");
					return;
				}
			}

			for (int i = 0; i < numberOfUsers; i++) {
				if (strcmp(nameOfFriend, users[i].username) == 0) {
					char filename[NAME_LEN];
					snprintf(filename, sizeof(filename), "%sMembers.csv", currentGroup->groupName);
					writeGroupMemberToFile(nameOfFriend, "Member");
					updateGroups();
					printf("Added %s to the group.\n\n", nameOfFriend);
					groupMenu(currentGroup);
					return;
				}
			}
			printf("This person does not exist.\n\n");
		}
	} else {
        printf("Group is full.\n\n");
		return;
    }
}

void leaveGroup(Group *currentGroup) {
	char filename[NAME_LEN];
	snprintf(filename, sizeof(filename), "%sMembers.csv", currentGroup->groupName);
	removeGroup(filename, currentGroup->groupName);
	updateGroups();
	printf("You have left the group.\n\n");
}

void chatToGroup(Group *currentGroup) {
	while (1) {
		printf("1) Message\n");
		printf("2) Go Back\n");

		promptUserForInput("%d", &choice);
		
		switch (choice) {
			case 1:
				for (int i = 0; i < currentGroup->amountOfMembers; i++) {
					if (strcmp(myName, currentGroup->members[i].name) == 0) {
						if (currentGroup->members[i].muted) {
							printf("You are muted.\n\n");
							return;
						}
					}
				}

				while (1) { 
    				printf("Type your message\n");
        			promptUserForString(message);
        			if (strlen(message) >= MSG_LEN) {
        				printf("Message exceeded limit\n\n");
        			} else {
            			break;
        			}
				}
				printf("me -> %s\n", message);	
				break;
			case 2:
				return;
			default:
				printf("Invalid input, please choose 1 or 2\n");
				break;
		}
	}
}		

void privateMsg(char *nameOfFriend) {
	while (true) {
		printf("Type your message\n");
		promptUserForString(message);
		if (strlen(message) >= MSG_LEN) {
			printf("Message exceeded limit, Please try again\n\n");
		} else {
			break;
		}
	}
	printf("me -> %s) %s\n\n", nameOfFriend, message);
}

void promoteMember(Member *member) {
	if (strcmp(member->role, "Member") == 0) {
		strcpy(member->role, "Admin");
		printf("%s has been promoted to %s.\n\n", member->name, member->role);
	} else {
		printf("%s is already an admin.\n\n", member->name);
	}
}

void muteMember(Member *member) {
	
	if (strcmp(member->name, myName) == 0) {
		printf("You cannot mute yourself.\n\n");
		return;
	}

	if (strcmp(member->role, "Admin") == 0) {
		printf("You cannot mute an admin.\n\n");
		return;
	}

	member->muted = true;
	printf("You have muted %s.\n\n", member->name);
	return;
}

void groupMemberMenu(Group *currentGroup, int curMember) {
	while (true) {
		printf("1) Message privately\n");
		printf("2) Promote this person\n");
		printf("3) Mute this person\n");
		printf("4) Kick this person\n");
		printf("5) Go back\n");
		promptUserForInput("%d", &choice);
		
		Member *member = &currentGroup->members[curMember - 1];

		switch (choice) {
			case 1:
				privateMsg(member->name);
				break;
			case 2:
				promoteMember(member);
				break;
			case 3:
				muteMember(member);
				break;
			case 4:
				kickFromGroup(member, currentGroup);
			case 5:
				groupMemberList(currentGroup);
				break;
			default:
				printf("Please select a valid option.\n\n");
				break;
		}
	}
}

void groupMemberList(Group *currentGroup) {
	int i;
	printf("Current group members\n\n");
	for	(i = 0; i < currentGroup->amountOfMembers; i++) {
		printf("%d) %s %s : ", i + 1, currentGroup->members[i].role, currentGroup->members[i].name);	
		for (int j = 0; j < numberOfUsers; j++) {
			if (strcmp(currentGroup->members[i].name, users[j].username) == 0){
				printf("%s\n", users[j].status);
			}
		}
	}
	printf("%d) Go back\n", i + 1);
	promptUserForInput("%d", &choice);
	if (choice == i + 1) {
		groupMenu(currentGroup);
	}

	groupMemberMenu(currentGroup, choice);
}

int groupMenu(Group *currentGroup) {
	while (1) {
		printf("1) Chat to the group\n");	
		printf("2) Add to the group\n");
		printf("3) Leave the group\n");
	    printf("4) See group members\n");
		printf("5) Go back\n");
		promptUserForInput("%d", &choice);

	 	switch (choice) {
       		case 1:
				chatToGroup(currentGroup);
				break;
			case 2:
         		addToGroup(currentGroup);
				break;
			case 3:
				leaveGroup(currentGroup);
				groupsMenu();
				break;
         	case 4:
            	groupMemberList(currentGroup);
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

