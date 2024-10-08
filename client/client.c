#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"
#include <stdbool.h>

char myPass[NAME_LEN];

void start(void);
void registerUser(void);
void login(void);
int menu(void);
void chatWithRandom();

int main(void) {
	registerAllTypes();
	updateUsersArray();	
	updateGroups();
	for (int i = 0; i < numberOfFriends; i++) {
		printf("friend %d = %s", i + 1, friendList[i]);
	}
	printf("\nnumber of users = %d\n\n", numberOfUsers);
	for (int i = 0; i < numberOfUsers; i++) {
		printf("User %d = %s\n", i, users[i].username);
		printf("Pass %d = %s\n", i, users[i].password);
		printf("status %d = %s\n", i, users[i].status); 	
		printf("\n"); 
	}
	clientSocket();
	start();
	updateFriendArray(myName);
	updateFReq(myName);
	menu();
	return 0;
}



void start(void) {
    while (true) {
        printf("1) Login\n");
        printf("2) Register\n");
        
		promptUserForInput("%d", &choice);
		
        switch (choice)	{
			case 1: 
				login();
				return;
			case 2:
				registerUser();
				return;
 			default:
				printf("Invalid choice. Please press 1 or 2.\n\n");
				break;
		}
	}

    return;
}

int menu(void) {
	while (1) {
        printf("1) Chat with a random\n");
        printf("2) Friends\n");
		printf("3) Groups\n");
        printf("4) Settings\n");
        printf("5) Quit\n");

		promptUserForInput("%d", &choice);		

		switch (choice) {
    	case 1:
			chatWithRandom();
			break;
	  	case 2:
        printf("Showing friends menu...\n\n");
                friends();
				break;
			case 3:
				printf("Showing groups menu...\n\n");
				groupsMenu();
				break;
            case 4:
                printf("Opening settings...\n\n");
				settingsMenu();
                break;
            case 5:
                printf("Goodbye!\n");
				S data = form_s_struct(myName);
				serializeMessage(ClientDisconnect, &data);
				updateUsersArray();
                exit(0);
            default:
                printf("Invalid choice. Please select a valid option.\n\n");
                break;
        }
    }

    return 0;
}

void chatWithRandom() {
	for (int i = 0; i < numberOfUsers; i++) {
		if (strcmp(users[i].username, myName) == 0) {
		} else if (strcmp(users[i].status, "Online") == 0) {
			printf("Starting chat with random...\n");
			printf("You are now chatting with %s.\n\n", users[i].username);
		} else {
			printf("Noone is online at the moment.\n\n");
			menu();
		}
	}
}


void login(void) {
	while (true) {
		if (numberOfUsers == 0) {
			printf("There are no users.\n\n");
			exit(0);
		}

        printf("Username\n");
   
        if (isValidName(myName)) {
    		printf("Password\n");
       	    if (isValidPass(myPass)) {
				for (int i = 0; i < numberOfUsers; i++) {
              		if (strcmp(users[i].username, myName) == 0 && strcmp(users[i].password, myPass) == 0) {
						S data = form_s_struct(myName);
						serializeMessage(LoginUserRequest, &data);
						printf("sent server login data\n");
						updateUsersArray();
                    	return;	
					}
    			}    
				printf("Invalid username or password\n\n");
	  		}
       	} 
	}
}

void registerUser(void) {
	while (1) {
		printf("What would you like your username to be?\n");
			
    	if (isValidName(myName)) {
			int isAvailable = 1;
			for (int i = 0; i < numberOfUsers; i++) {
            	if (strcmp(users[i].username, myName) == 0) {
                	printf("Name is unavailable, Please select another name.\n\n");
					isAvailable = 0;
					break;
        		}
			}
			if (isAvailable) {
				while (1) {
					printf("What would you like your password to be?\n");
					if (isValidPass(myPass)) {
						SP data = form_sp_struct(myName, myPass);
						serializeMessage(RegisterUserRequest, &data);
        				printf("Welcome %s\n\n", myName);
						updateUsersArray();
						return;
   					} 
				}
			}
		} 
	}
}



