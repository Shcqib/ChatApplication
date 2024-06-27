#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"

char nameOfFriend[50];
int choice = 0;

void promptUserForInput(const char *type, void *input) {
	printf(": ");
	scanf(type, input);
	printf("\n");
}

void promptUserForString(char *string) {
	int c;
    while ((c = getchar()) != '\n' && c != EOF);

	printf(": ");
	fgets(string, 100, stdin);
	string[strcspn(string, "\n")] = '\0';
	printf("\n");
}

int isValidName(const char *name) {
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
