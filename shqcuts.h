#include <stdbool.h>

#ifndef SHQCUTS_H
#define SHQCUTS_H

#define MAX_LINE_LENGTH 256
#define MAX_GROUPS 2
#define MAX_MEMBERS 5

#define MAX_USERS 5
#define MAX_FRIENDS 5
#define MSG_LEN 100
#define NAME_LEN 20

void writeGroupToFile(char *groupName);
void updateStatus(char *name);
int main(void);
void initializeGroupFile(char *username);
void initializeFile(char *filename, const char *username);
FILE *openFile(char *filename, char *format);
void writeFriendToFile(char *filename, char *username, char *friendName);
void acceptAllFReq();
void removeAllFReq();
void acceptFReq(char *name);
void removeFReq(char *name);
void removeUser(char *name);
void replaceUsername(char *name, char *prevName);
void clientSocket();
void sendMessage(char *bufferToSend);
void removeGroup(char *filename, char *groupname);
void replacePassword(char *name, char *pass);
void writeGroupMemberToFile(char *groupName, char *name);
void removeMember(char *name, char *filename);
void updateFReq(char *name);
void writeToFile(char *filename, char *name);
void writeUserToFile(char *filename, char *name, char *pass);
void updateUsersArray(void);
void addGroupToArray(char *name);
void removeFriendFromArray(char *name);
void updateFriendArray(char *name);
void updateGroups();
void updateGroupArray(char *filename);
void addFriendToArray(char *username, char *friendname);
void promptUserForString(char *string);
void addGroupMemberToArray(char *name, char *groupName);
int menu(void);
int isValidPass(char *pass);
int isValidName(char *name);
void promptUserForInput(const char *type, void *input);
int friends(void);
int groupsMenu(void);
void settingsMenu(void);

extern int lenOfMsg; 
extern char myPass[NAME_LEN];
extern char message[MSG_LEN];
extern char nameOfFriend[NAME_LEN];
extern int choice;
extern char myName[NAME_LEN];
extern int sockfd;
extern int numberOfFriends;
extern char friendList[MAX_FRIENDS][NAME_LEN];
extern int numOfMembers;
extern bool active;
extern int numberOfGroups;
extern char friendRequests[MAX_FRIENDS][NAME_LEN];
extern int numOfFRequests;
extern int userCount;
extern int numLines;
extern char headers[MAX_USERS][NAME_LEN];

typedef struct {
    char name[NAME_LEN];
    bool muted;
    char role[NAME_LEN];
} Member;

typedef struct {
    char groupName[NAME_LEN];
    Member members[MAX_MEMBERS];
    int amountOfMembers;
   
} Group;

Group groups[MAX_GROUPS];

typedef struct {
	char username[NAME_LEN];
	char password[NAME_LEN];
	char status[10];
} User;

extern int numberOfUsers;
User users[MAX_USERS];

int searchUsername(char *name);
int searchPassword(char *pass, char *name);

#endif

