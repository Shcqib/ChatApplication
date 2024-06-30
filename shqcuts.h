#ifndef SHQCUTS_H
#define SHQCUTS_H

#define MSG_LEN 100

void writeUserToFile(char *name, char *pass);
void addUsersFromFile(void);
void addUser(char *name, char *pass);
void promptUserForString(char *string);
int menu(void);
int isValidPass(char *pass);
int isValidName(char *name);
void promptUserForInput(const char *type, void *input);
int friends(void);
int groupsMenu(void);

extern char message[MSG_LEN];
extern char nameOfFriend[50];
extern int choice;
extern char myName[50];

typedef struct {
	char username[50];
	char password[50];
} User;

extern int numberOfUsers;
User users[50];

#endif

