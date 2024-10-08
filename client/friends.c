#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "shqcuts.h"

#define MAX_FRIENDS 5

void removeFriend(void);
void messageFriend(void);
void showFriendRequests();
void showFriendList(void);
void addFriend(void);

char message[MSG_LEN];

int friends(void) {

    while (1) {
		updateFriendArray(myName);

        printf("1) See all friends\n");
        printf("2) Add a friend\n");
        printf("3) Remove a friend\n");
        printf("4) Message a friend\n");
		printf("5) Friend Requests\n");
		printf("6) Go back\n");

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
				showFriendRequests();
				break;
			case 6:
				menu();
				break;
            default:
                printf("Invalid choice. Please select a valid option.\n\n");
                break;
        }
    }
}

void fReqMenu(char *friendName) {
	updateFReq(myName);

	while (true) {
		printf("1) Accept friend request.\n");
		printf("2) Decline friend request.\n");
		printf("3) Go back.\n");
		promptUserForInput("%d", &choice);

		switch (choice) {
			case 1: {
				SR data = form_sr_struct(myName, friendName);
				serializeMessage(AddFriendRequest, &data);
				friends();
			}
			case 2: {
				SR data = form_sr_struct(myName, friendName);
				serializeMessage(RemoveFriendRequest, &data);
				friends();
			}
			case 3:
				friends();
			default:
				printf("Invalid choice please try again.\n\n");
				break;
		}
	}
}

void showFriendRequests() {
    updateFReq(myName);

    while (true) {
        if (numOfFRequests == 0) {
            printf("You have no friend requests.\n\n");
            friends();
            return;  
        }

        int i;
        for (i = 0; i < numOfFRequests; i++) {
            printf("%d) %s has sent you a friend request.\n", i + 1, friendRequests[i]);
        }
        printf("%d) Accept all friend requests.\n", i + 1);
        printf("%d) Decline all friend requests.\n", i + 2);
        printf("%d) Go back.\n", i + 3);
        
        int choice;
        promptUserForInput("%d", &choice);

        if (choice == i + 1) {
            printf("Accepted all friend requests.\n\n");
			return;
		} else if (choice == i + 2) {
            printf("Declined all friend requests.\n\n");
			return;
        } else if (choice == i + 3) { 
			friends();
        } else if (choice >= 1 && choice <= i) {
            fReqMenu(friendRequests[choice - 1]);  
        } else {
            printf("Invalid choice, please try again.\n\n");
        }
    }
}

void messageFriend(void) {
	int lenOfMsg = 0;

	while (1) {
		printf("Type the name of the friend you would like to message\n");
		if (isValidName(nameOfFriend)) {
			int found = 0;
			for (int i = 0; i < numberOfFriends; i++) {
				if (strcmp(nameOfFriend, friendList[i]) == 0) {
					found = 1;
					while (1) {
						printf("Type your message\n");
						promptUserForString(message);
						lenOfMsg = strlen(message);
						printf("%d\n\n", lenOfMsg);
						if (lenOfMsg >= MSG_LEN) {
							printf("message limit exceeded.\n\n");
							return;
						} else {
							break;	
						}
					}

					SRM data = form_srm_struct(myName, nameOfFriend, message);
					printf("message = %s", data.Message);
					serializeMessage(SendMessageRequest, &data);
					return;
				}
			}	
			if (!found) {
				printf("You are not friends with that person.\n\n");
			}	
		}
	}
}

void removeFriend(void) {
	while (1) {
    	printf("What friend would you like to remove?\n");
		if (isValidName(nameOfFriend)) {
			int found = 0;
			for (int i = 0; i < numberOfFriends; i++) {
				if (strcmp(nameOfFriend, friendList[i]) == 0) {
					SR data = form_sr_struct(myName, nameOfFriend);
					serializeMessage(RemoveFriend, &data);
					return;
				}
			}
			if (!found) {
				printf("You are not friends with that person.\n\n");
			}	
		}
	}
}


void addFriend(void) {
	if (numberOfFriends < MAX_FRIENDS) {
		while (1) {
       		printf("What is the name of your friend?\n");
			if (isValidName(nameOfFriend)) {
				if (strcmp(nameOfFriend, myName) == 0) {
					printf("You cannot add yourself as a friend.\n\n");
					return;
				}

				for (int i = 0; i < numberOfFriends; i++) {
					if (strcmp(friendList[i], nameOfFriend) == 0) {
						printf("You are already friends with this person.\n\n");
						return;
					}
				}

				int userIndex = updateFile(friendReqFilePath, myName);

				for (int i = 0; i < numberOfFriends; i++) {
					if (strcmp(friendsArray[i][userIndex], nameOfFriend) == 0) {
						printf("You have already sent this person a friend request.\n\n");
						return;
					}
				}

				bool userExists = false;
			 	for (int i = 0; i < numberOfUsers; i++) {
                    if (strcmp(users[i].username, nameOfFriend) == 0) {
						SR data = form_sr_struct(myName, nameOfFriend);
						serializeMessage(SendFriendRequest, &data);
						userExists = true;
                        return;
                    }
                }

				if (!userExists) {
					printf("This person does not exist.\n\n");
				}

			} else { 
				printf("Friend list is full.\n\n");
			}	
		}
	}
}

void showFriendList(void) {
	if (numberOfFriends == 0) {
		printf ("You have no friends.\n\n");
	}
	else { 
		printf("Your Friends...\n\n");
		for (int i = 0; i < numberOfFriends; i++) {
    		printf("%d) %s ", i + 1, friendList[i]);
			for (int j = 0; j < numberOfUsers; j++) {
				if (strcmp(users[j].username, friendList[i]) == 0) {
					printf("%s\n", users[j].status);
				}
			}
		}
		printf("\n");
	}
}
