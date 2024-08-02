#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shqcuts.h"

char myName[NAME_LEN];

int typeCount = 0;

void registerType(int typeIndex, size_t dataSize) {
    printf("Registering type %d\n", typeIndex);
    if (typeCount < MAX_TYPES) {
        typeRegistry[typeCount].typeIndex = typeIndex;
        typeRegistry[typeCount].size = dataSize;
        typeCount++;
        printf("Type count %d\n", typeCount);
    }
}

void registerAllTypes() {
	registerType(Connect, sizeof(S));
    registerType(SendFriendRequest, sizeof(SRM));
    registerType(SendMessageRequest, sizeof(SRM));
}
