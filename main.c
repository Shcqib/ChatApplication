#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

char myName[50];

int start(void);
void registerUser(void);
void login(void);
int menu(void);

int main(void) {	
	start();
	menu();
	return 0;
}

void login(void) {
	while (1) {
		printf("What is your Username?\n");
  		promptUserForString(myName);
    	if (isValidName(myName)) { 
       		printf("Welcome back %s\n\n", myName);
			break;
    	} else {
    		printf("Invalid name, Please select another name.\n\n");
    	}
	}  
}

void registerUser(void) {
	printf("Username?\n");
    promptUserForInput("%s", myName);
    if (isValidName(myName)) {
        printf("Welcome %s\n\n", myName);
    } else {
        printf("Invalid name, Please select another name.\n\n");
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
                printf("Showing friends list...\n\n");
                friends();
				break;
			case 3:
				printf("Groups...\n\n");
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

