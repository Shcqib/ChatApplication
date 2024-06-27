#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

#define MAX_FRIENDS 5

void removeFriend(void);
void messageFriend(void);
char friendList[MAX_FRIENDS][50];
int numberOfFriends = 0;

void showFriendList(void);
void addFriend(void);

int friends(void) {

    while (1) {
        printf("1) See all friends\n");
        printf("2) Add a friend\n");
        printf("3) Remove a friend\n");
        printf("4) Message a friend\n");
		printf("5) Go back\n");

        promptUserForInput("%d", &choice);

        switch (choice) {
            case 1:
				showFriendList();
                break;
            case 2:
				addFriend();
                break;
            case 3:
				removeFriend();
                break;
            case 4:
				messageFriend();
                break;
			case 5:
				menu();
				break;
            default:
                printf("Invalid choice. Please select a valid option.\n\n");
                break;
        }
    }
}

void messageFriend(void) {
	char message[100];
	printf("Type the name of the friend you would like to message\n");
	promptUserForInput("%s", nameOfFriend);
	int found = 0;
	for (int i = 0; i < numberOfFriends; i++) {
		if (strcmp(nameOfFriend, friendList[i]) == 0) {
			found = 1;
			printf("Type your message\n");
			promptUserForInput("%s", message);
			printf("me -> %s) %s\n\n", nameOfFriend, message); 
			return;
		}
		
		if (!found) {
			printf("You are not friends with that person.\n\n");
		}	
	}
}

void removeFriend(void) {
	printf("What friend would you like to remove?\n");
	promptUserForInput("%49s", nameOfFriend);
	if (isValidName(nameOfFriend)) {
		int found = 0;
		for (int i = 0; i < numberOfFriends; i++) {
			if (strcmp(nameOfFriend, friendList[i]) == 0) {
                found = 1;
                for (int j = i; j < numberOfFriends - 1; j++) {
                    strcpy(friendList[j], friendList[j + 1]);
                }
                numberOfFriends--;
                printf("%s is no longer your friend.\n\n", nameOfFriend);
                break;
            }
        }
       	if (!found) {
           	printf("You are not friends with that person.\n\n");
        }	
	}
}

void addFriend(void) {
	if (numberOfFriends < MAX_FRIENDS) {
		printf("What is the name of your friend?\n");
    	promptUserForInput("%49s", nameOfFriend);
		if (isValidName(nameOfFriend)) {
   			strcpy(friendList[numberOfFriends], nameOfFriend);
    		numberOfFriends++;
        	printf("Friend added.\n\n");
    	} else { 
        	printf("Friend list is full.\n\n");
    	}	
	}
	else {
		printf("Invalid name.\n\n");
	}
}

void showFriendList(void) {
	
	if (numberOfFriends == 0) {
		printf ("You have no friends.\n\n");
	}
	else { 
		printf("Your Friends...\n\n");
		for (int i = 0; i < numberOfFriends; i++) {
    		printf("%d) %s\n", i + 1, friendList[i]);
		}
		printf("\n");
	}
}
