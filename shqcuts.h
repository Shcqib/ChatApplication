#ifndef SHQCUTS_H
#define SHQCUTS_H

void promptUserForString(char *string);
int menu(void);
int isValidName(const char *name);
void promptUserForInput(const char *type, void *input);
int friends(void);
int groupsMenu(void);

extern char nameOfFriend[50];
extern int choice;
extern char myName[50];

#endif

