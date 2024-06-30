#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

char myName[50];
char Pass[50];

int start(void);
void registerUser(void);
void login(void);
int menu(void);

int main(void) {
	addUsersFromFile();	
	printf("number of users = %d\n\n", numberOfUsers);
	for (int i = 0; i < numberOfUsers; i++) {
		printf("User %d = %s\n", i, users[i].username);
		printf("Pass %d = %s\n", i, users[i].password);
		printf("\n"); 
	}
	start();
	menu();
	return 0;
}

void login(void) {
	while (1) {
        printf("Username\n");
   
        if (isValidName(myName)) {
    		printf("Password\n");
       	    if (isValidPass(Pass)) {
				for (int i = 0; i < numberOfUsers; i++) { 
              		if (strcmp(users[i].username, myName) == 0 && strcmp(users[i].password, myName) == 0) {
	           			printf("Welcome back %s\n\n", myName);
                    	return;	
					} else {
						printf("Invalid username or password\n\n");
						break;
					}
          		}
			} else {
           		printf("Password cannot contain spaces, Please try again.\n\n");
          	}
       	} else {
       		printf("Invalid name, Please select another name.\n\n");
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
                	printf("myName is unavailable, Please select another name.\n\n");
					isAvailable = 0;
					break;
        		}
			}
			if (isAvailable) {
				while (1) {
					printf("What would you like your password to be?\n");
					if (isValidPass(Pass)) {
        				printf("Welcome %s\n\n", myName);
						writeUserToFile(myName, Pass);
						return;
   					} else {
						printf("Password cannot contain spaces, Please try again.\n\n");
					}
				}
			}
		} else {
        	printf("Invalid name, Please select another name.\n\n");
    	}
	}
}

int start(void) {
    while (1) {
        printf("1) Login\n");
        printf("2) Register\n");
        
		promptUserForInput("%d", &choice);
		
        switch (choice)	{
			case 1: 
				login();
				return 0;
				break;
			case 2:
				registerUser();
				return 0;
				break;
 			default:
				printf("Invalid choice. Please press 1 or 2.\n\n");
				break;
		}
	}

    return 0;
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
                printf("Starting chat with a random...\n\n");
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
                break;
            case 5:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please select a valid option.\n\n");
                break;
        }
    }

    return 0;
}

