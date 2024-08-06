#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

char myName[NAME_LEN];
bool active;

int typeCount = 0;

FILE *openFile(char *filename, char *format) {
    FILE *file = fopen(filename, format);
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    return file;
}

void registerType(int typeIndex, size_t dataSize) {
    if (typeCount < MAX_TYPES) {
        typeRegistry[typeCount].typeIndex = typeIndex;
        typeRegistry[typeCount].size = dataSize;
        typeCount++;
    }
}

void registerAllTypes() {
	registerType(Connect, sizeof(S));
    registerType(SendFriendRequest, sizeof(SR));
    registerType(SendMessageRequest, sizeof(SRM));
	registerType(AddFriendRequest, sizeof(SR));
	registerType(RemoveFriendRequest, sizeof(SR));
	registerType(RegisterUserRequest, sizeof(SP));
	registerType(LoginUserRequest, sizeof(S));
	registerType(ClientDisconnect, sizeof(S));
	registerType(ReplaceUsernameRequest, sizeof(ReplaceUsernameData));
	registerType(ReplacePasswordRequest, sizeof(SP));
	registerType(DeactivateAccountRequest, sizeof(S));
	registerType(RemoveFriend, sizeof(SR));
}
