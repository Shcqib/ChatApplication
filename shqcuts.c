#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"

int numberOfUsers = 0;
char nameOfFriend[50];
int choice = 0;

int c;

void promptUserForInput(const char *type, void *input) {
	printf(": ");
	scanf(type, input);
	printf("\n");

	while ((c = getchar()) != '\n' && c != EOF);
}

void promptUserForString(char *string) {
	printf(": ");
	fgets(string, 250, stdin);
	string[strcspn(string, "\n")] = '\0';
	printf("\n");
}


int isValidName(char *name) {
	promptUserForString(name);

    while (*name) {
        if (isspace((unsigned char)*name)) {
            return 0;
        }
        if (!isalnum((unsigned char)*name)) {
            return 0;
        }
        name++;
    }
	return 1;
}

int isValidPass(char *pass) {
	promptUserForString(pass);

    while (*pass) {
        if (isspace((unsigned char)*pass)) {
            return 0;
        }
        pass++;
    }
    return 1;
}

void writeUserToFile(char *name, char *pass) {
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "%s %s\n", name, pass);
    fclose(file);
}

void addUsersFromFile(void) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
	
	char name[50];
	char pass[50];

    while (fscanf(file, "%s %s", name, pass) != EOF) {
   		strcpy(users[numberOfUsers].username, name);
    	strcpy(users[numberOfUsers].password, pass);
    	numberOfUsers++;
	}

    fclose(file);
}
