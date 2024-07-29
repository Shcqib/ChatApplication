#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "serverFunctions.h"
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY
    };

	bind(sockfd, (struct sockaddr *)&address, sizeof(address));

    listen(sockfd, 10);

    struct pollfd fds[MAX_CLIENTS + 1];  
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    int num_clients = 0;

    for (;;) {
        char buffer[BUFFER_SIZE] = {0};

        fds[1].fd = 0;
        fds[1].events = POLLIN;

        int poll_count = poll(fds, num_clients + 2, -1); 

        if (fds[0].revents & POLLIN) {
            int clientfd = accept(sockfd, NULL, NULL);
			if (num_clients < MAX_CLIENTS) {
				num_clients++;
				fds[num_clients + 1].fd = clientfd;  
				fds[num_clients + 1].events = POLLIN;
			}
            
        }

        if (fds[1].revents & POLLIN) {
            read(0, buffer, BUFFER_SIZE - 1);
            buffer[strlen(buffer) - 1] = '\0';
            for (int i = 2; i <= num_clients + 1; i++) {
                send(fds[i].fd, buffer, strlen(buffer) + 1, 0);
            }
        }

        for (int i = 2; i <= num_clients + 1; i++) {
            if (fds[i].revents & POLLIN) {
                int bytes_received = recv(fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
                if (bytes_received <= 0) {
                    close(fds[i].fd);
                    fds[i] = fds[num_clients + 1]; 
                    num_clients--;
                    i--;  
                } else {
                    buffer[bytes_received] = '\0';
                    printf("%s\n", buffer);
					printf("Received from client %d: %s\n", fds[i].fd, buffer);
                    handleCommand(buffer, fds[i].fd);
                }
            }
        }
    }

    return 0;
}
