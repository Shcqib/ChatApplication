#include <stdbool.h>

#ifndef SHQCUTS_H
#define SHQCUTS_H

#define dataPath "data/"                                                                                  
#define usersFilePath dataPath "users.csv" 
#define groupFilePath dataPath "groups.csv"
#define friendsFilePath dataPath "friends.csv" 
#define friendReqFilePath dataPath "friendReq.csv"

#define MAX_LINE_LENGTH 256
#define MAX_GROUPS 2
#define MAX_MEMBERS 5

#define MAX_TYPES 20
#define MAX_USERS 5
#define MAX_FRIENDS 5
#define MSG_LEN 256
#define NAME_LEN 20
#define BUFFER_SIZE 1024

void writeGroupToFile(char *groupName, char *name);
void updateStatus(char *name, bool status);
int main(void);
void initializeFile(char *filename, const char *username);
FILE *openFile(char *filename, char *format);
void writeFriendToFile(char *filename, char *username, char *friendName);
void acceptAllFReq();
void removeAllFReq();
void acceptFReq(char *name, char *friendName);
void removeFReq(char *name, char *friendName);
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
void writeUserToFile(char *filename, char *name, char *pass, bool status);
void updateUsersArray(void);
void registerType(int typeIndex, size_t dataSize);
void registerAllTypes();
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
int updateFile(char *filename, char *username);
void promptUserForInput(const char *type, void *input);
int friends(void);
int groupsMenu(void);
void settingsMenu(void);
void updateGroupMemberFile(int userIndex);
int updateGroupFile(char *username);

extern char friendsArray[MAX_FRIENDS][MAX_USERS][NAME_LEN];
extern int lenOfMsg; 
extern char myPass[NAME_LEN];
extern char message[MSG_LEN];
extern char nameOfFriend[NAME_LEN];
extern int choice;
extern char myName[NAME_LEN];
extern int sockfd;
extern int typeCount;
extern int numberOfFriends;
extern char friendList[MAX_FRIENDS][NAME_LEN];
extern int numOfMembers;
extern bool active;
extern int numberOfGroups;
extern char line[MAX_LINE_LENGTH];
extern char friendRequests[MAX_FRIENDS][NAME_LEN];
extern char groupsArray[MAX_GROUPS][MAX_USERS][NAME_LEN];
extern char membersArray[MAX_MEMBERS][MAX_GROUPS][NAME_LEN];
extern int numOfFRequests;
extern int userCount;
extern int numLines;
extern char headers[MAX_USERS][NAME_LEN];

typedef enum {
	Connect = 1,	
    SendFriendRequest = 2,
    SendMessageRequest = 3,
	AddFriendRequest = 4,
	RemoveFriendRequest = 5,
	RegisterUserRequest = 6,
	LoginUserRequest = 7,
	ClientDisconnect = 8,
	ReplaceUsernameRequest = 9,
	ReplacePasswordRequest = 10,
	DeactivateAccountRequest = 11,
	RemoveFriend = 12,
	CreateGroupRequest = 13,
	AddToGroupRequest = 14,
} MessageType;

typedef struct {
    int typeIndex;
    size_t size;
} TypeInfo;

TypeInfo typeRegistry[MAX_TYPES];

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

typedef struct {
    char SenderName[NAME_LEN];
    char ReceiverName[NAME_LEN];
    char Message[MSG_LEN];
} SRM;

typedef struct {
	char SenderName[NAME_LEN];	
} S;

typedef struct {
	char SenderName[NAME_LEN];
	char ReceiverName[NAME_LEN];
} SR;

typedef struct {
	char SenderName[NAME_LEN];
	char SenderPass[NAME_LEN];
} SP;

typedef struct {
	char SenderName[NAME_LEN];
	char GroupName[NAME_LEN];
} SG;

typedef struct {
	char SenderName[NAME_LEN];
	char ReceiverName[NAME_LEN];
	char GroupName[NAME_LEN];
} SRG;

typedef struct {
	char SenderName[NAME_LEN];
	char PreviousName[NAME_LEN];
} ReplaceUsernameData;

extern int numberOfUsers;
User users[MAX_USERS];

int searchUsername(char *name);
void serializeMessage(MessageType reqType, void *data);
int searchPassword(char *pass, char *name);
SR form_sr_struct(char *senderName, char *receiverName);
S form_s_struct(char *senderName);
SRM form_srm_struct(char *senderName, char *receiverName, char *message);
SG form_sg_struct(char *senderName, char *groupName);
SRG form_srg_struct(char *senderName, char *receiverName, char *groupName);
SP form_sp_struct(char *senderName, char *senderPass);
ReplaceUsernameData form_replaceUser_struct(char *senderName, char *previousName);

#endif

