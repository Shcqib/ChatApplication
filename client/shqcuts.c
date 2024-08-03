#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "shqcuts.h"
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define dataPath "data/"                                                                                  
#define usersFilePath dataPath "users.csv" 

int numberOfUsers = 0;
char nameOfFriend[NAME_LEN];
int choice = 0;
char nameInFile[NAME_LEN], passInFile[NAME_LEN], statusInFile[NAME_LEN];
int sockfd = 0;
int c;
unsigned char buffer[1024];

void *receiveMessageFromServer(void *arg);
void clientSocket() {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);                                                                                                                                                                     
                                                                                                                                                                                                                      
    struct sockaddr_in address = {                                                                                                                                                                                    
        .sin_family = AF_INET,                                                                                                                                                                                        
        .sin_port = htons(9999),                                                                                                                                                                                      
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };                                                                                                                                                                                                                
                                                                                                                                                                                                                      
    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == 0) {
		S data;
		strcpy(data.SenderName, myName);
		serializeMessage(Connect, &data);
	}		

	pthread_t recv_thread;
	if (pthread_create(&recv_thread, NULL, receiveMessageFromServer, NULL) != 0) {
		perror("pthread_create");
		close(sockfd);
		exit(1);
	}
}	

void sendMessage(char *t) {

}

void serializeMessage(MessageType reqType, void *data) {                                                  
    printf("Sending type of %d\n", reqType);                                                              
    unsigned char buffer[BUFFER_SIZE];                                                                    
    buffer[0] = '\0';                                                                                     
                                                                                                          
    for (int i = 0; i < typeCount; i++) {                                                                 
        if (reqType == typeRegistry[i].typeIndex) {                                                       
            buffer[0] = (unsigned char)typeRegistry[i].typeIndex;                                         
            buffer[1] = (unsigned char)typeRegistry[i].size;                                              
            memcpy(buffer + 2, data, typeRegistry[i].size);                                               
                                                                                                          
            send(sockfd, buffer, typeRegistry[i].size + 2, 0);                                            
            printf("sent buffer to server %s\n", buffer);                                                 
        }                                                                                                 
    }                                                                                                     
}
void *receiveMessageFromServer(void *arg) {
    while (true) {
        char buffer[BUFFER_SIZE] = {0};

        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received < 0) perror("recv");
            close(sockfd);
            return NULL;
        }

        buffer[bytes_received] = '\0'; 
        printf("%s\n", buffer);
    }

    return NULL;
}
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
        if (isspace((unsigned char)*name) || !isalnum((unsigned char)*name)) {
        	printf("Invalid name, please try again.\n\n"); 
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
            printf("Invalid Password, cannot contain spaces.\n\n");
		return 0;
        }
        pass++;
    }
    return 1;
}

FILE *openFile(char *filename, char *format) {
	FILE *file = fopen(filename, format);
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

	return file;
}

void writeUserToFile(char *filename, char *name, char *pass) { 
    FILE *file;
    if ((file = openFile(filename, "a")) == NULL) return;

    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fprintf(file, "username,password,status\n");
    }

    fseek(file, 0, SEEK_SET);

    if (active) {
        fprintf(file, "%s,%s,Online\n", name, pass);
    } else {
        fprintf(file, "%s,%s,Offline\n", name, pass);
    }
    fclose(file);

	initializeGroupFile(name);
    initializeFile("data/friends.csv", name);
    initializeFile("data/friendReq.csv", name);
}

void replacePassword(char *name, char *pass) {
	FILE *file, *tempFile;
	if  ((file = openFile(usersFilePath, "r")) == NULL) return;
	if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    int found = 0;
	fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        writeUserToFile("tempUsers.csv",	name, pass);
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
        printf("Password successfully changed.\n\n");
    } else {
        remove("tempUsers.csv");
    }
}

void removeUser(char *name) {
	FILE *file, *tempFile;
    if  ((file = openFile(usersFilePath, "r")) == NULL) return;
    if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    int found = 0;
    fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
    } else {
        remove("tempUsers.csv");
    }
}

void removeMember(char *name, char *filename) {
    FILE *file, *tempFile;
    if  ((file = openFile(filename, "r+")) == NULL) return;
    if  ((tempFile = openFile("tempGroups.csv", "a")) == NULL) return;

    int found = 0;
    char nameInFile[NAME_LEN], role[NAME_LEN], groupName[NAME_LEN];

	if (fscanf(file, "%s\n", groupName) == EOF) {
        fclose(file);
        return; 
    } else if (fscanf(file, "%s\n", groupName) != EOF) {
		fprintf(tempFile, "%s\n", groupName);
	}

    while (fscanf(file, "%[^,],%s\n", role, nameInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s,%s\n", role, nameInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(filename);
        rename("tempGroups.csv", filename);
   } else {
        remove("tempGroups.csv");
    }
}


void removeFriendFromArray(char *name) {
	FILE *file, *tempFile;
	if  ((file = openFile("friends.csv", "r+")) == NULL) return;
	if  ((tempFile = openFile("tempFriends.csv", "a")) == NULL) return;

    int found = 0;
	char nameOfFriend[NAME_LEN];

    while (fscanf(file, "%[^\n]\n", nameOfFriend) != EOF) {
        if (strcmp(nameOfFriend, name) == 0) {
            found = 1;
        } else {
            fprintf(tempFile, "%s\n", nameOfFriend);
        }
    }
	
	fclose(file);
    fclose(tempFile);

    if (found) {
        remove("friends.csv");
        rename("tempFriends.csv", "friends.csv");
		updateFriendArray(myName);	
    } else {
        remove("tempFriends.csv");
    }
}

void replaceUsername(char *name, char *prevName) {
    FILE *file, *tempFile; 
	if  ((file = openFile(usersFilePath, "r+")) == NULL) return;
	if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;

    char pass[NAME_LEN];
	int found = 0;

	fprintf(tempFile, "username,password,status\n");
    fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, prevName) == 0) {
    		strcpy(pass, passInFile);
			found = 1;
		} else {
			fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
		}
    }

	fclose(file);
	fclose(tempFile);	

	if (found) {
		writeUserToFile("tempUsers.csv", name, pass);
		remove(usersFilePath);
		rename("tempUsers.csv", usersFilePath);
		printf("Username successfully changed.\n\n");	
    } else {
        remove("tempUsers.csv");
    }
}

void updateStatus(char *name) {
	FILE *file, *tempFile;
    if  ((file = openFile(usersFilePath, "r+")) == NULL) return;
    if  ((tempFile = openFile("tempUsers.csv", "a")) == NULL) return;	
	
	int found = 0;
	char pass[NAME_LEN];

	fprintf(tempFile, "username,password,status\n");
	fscanf(file, "%*[^\n]\n");

    while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
        if (strcmp(nameInFile, name) == 0) {
            strcpy(pass, passInFile);
			found = 1;
        } else {
            fprintf(tempFile, "%s,%s,%s\n", nameInFile, passInFile, statusInFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
		writeUserToFile("tempUsers.csv", name, pass);
		remove(usersFilePath);
        rename("tempUsers.csv", usersFilePath);
    } else {
        remove("tempUsers.csv");
    }
}

void updateUsersArray(void) {
	FILE *file;
	if  ((file = openFile(usersFilePath, "r+")) == NULL) return;

	int i = 0;
	fscanf(file, "%*[^\n]\n");

	while (fscanf(file, "%[^,],%[^,],%[^\n]\n", nameInFile, passInFile, statusInFile) != EOF) {
		strcpy(users[i].username, nameInFile);
		strcpy(users[i].password, passInFile);
		strcpy(users[i].status, statusInFile);
		i++;
	}

	numberOfUsers = i;
	fclose(file);
}

int searchUsername(char *name) {
	for (int i = 0; i < numberOfUsers; i++) {
		if (strcmp(users[i].username, name) == 0) {
			return 1;
		}
	}
	return 0;
}

int searchPassword(char *pass, char *name) {
    for (int i = 0; i < numberOfUsers; i++) {
		if (strcmp(users[i].username, name) == 0 && (strcmp(users[i].password, pass) == 0)) {
			return 1;
		}
    }
    return 0;
}
