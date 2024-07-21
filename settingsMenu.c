#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "shqcuts.h"

void details(void);
void account(void);
void logOut();
void deactivateAcc();

void settingsMenu(void) {
	while (1) {
		printf("1) Account\n");
		printf("2) Log out\n");
		printf("3) Go back\n");

		promptUserForInput("%d", &choice);
		switch (choice) {
			case 1:
				printf("Account...\n\n");
				account();
				break;
			case 2:
				printf("Logging out...\n\n");
				logOut();
				break;
			case 3:
				menu();
				break;
			default:
				printf("Invalid option.\n\n");
				break;
		}
	} 
}

void logOut() {
	active = false;
	updateStatus(myName);
	updateUsersArray();
	main();
}

void account(void) {
	while (1) {
		printf("1) Private details\n");
		printf("2) Deactivate account\n");
		printf("3) Go back\n");

		promptUserForInput("%d", &choice);
		switch (choice) {
			case 1:
				printf("Private details...\n\n");
				details();
				break;
			case 2:
				printf("Deactivating account...\n\n");
				deactivateAcc();
				break;
			case 3:
				settingsMenu();
				break;
			default:
                printf("Invalid option.\n\n");
                break;
		}
	}
}

void deactivateAcc() {
	printf("Are you sure you would like to deactivate your account, you will lose everything and you will be logged out.\n");
	printf("1) Yes\n");
	printf("2) No\n");

	promptUserForInput("%d", &choice);

	if (choice == 1) {
		removeUser(myName);
		logOut();
	} else if (choice == 2) {
		account();
	}
}

void switchUsername() {
	char prevName[NAME_LEN];
	
	while (1) {
   		printf("What would you like your new Username to be\n");
        strcpy(prevName, myName);
        if (isValidName(myName)) {
        	if (searchUsername(myName)) {
        		printf("This Username is taken.\n\n");
    		} else {
    	    	for (int i = 0; i < numberOfUsers; i++) {
	            	if (strcmp(users[i].username, prevName) == 0) {
                		replaceUsername(myName, prevName);
						updateUsersArray();
						break;
            		}
        		}
				break;
    		}
   		}
	}
}


void switchPassword() {
	char prevPass[NAME_LEN];
	while (1) {
		printf("Type your old password\n");
		if (isValidPass(prevPass)) {
			if (searchPassword(prevPass, myName)) {
				printf("What would you like your new Password to be.\n");
				if (isValidPass(myPass)) {
					for (int i = 0; i < numberOfUsers; i++) {
            			if (strcmp(users[i].username, myName) == 0 && strcmp(users[i].password, prevPass) == 0) {
                			replacePassword(myName, myPass);
							updateUsersArray();
							break;
        				}						
					}
					break;
				}
			} else {
				printf("Incorrect password, try again\n\n");
			}
		}
	}
}

void changePrivDetails(void) {
	printf("1) Change Username\n");
    printf("2) Change Password\n");
	printf("3) Go back\n");
    promptUserForInput("%d", &choice);
    switch (choice) {
		case 1:
			switchUsername();
			break;
		case 2:
        	switchPassword();
			break;
		case 3:
			details();
			break;
		default:
			printf("Invalid choice.\n\n");
			break;
	} 
}

void details(void) {
	while (1) {
        printf("1) See Private details\n");
        printf("2) Change Private details\n");
        printf("3) Go back\n");

        promptUserForInput("%d", &choice);
        switch (choice) {
            case 1:
                printf("Your Name - %s\n", myName);
                printf("Your Password - %s\n\n", myPass);
                break;
			case 2:
				changePrivDetails();
				break;
			case 3:
				account();
				break;
			default:
				printf("Invalid option.\n\n");
				break;
        }
    }
}
