/*
** EPITECH PROJECT, 2025
** ftp
** File description:
** client
*/

#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define CLASSIC_ERROR -1
#define FULL_READ 0
#define ERROR 84
#define PORT 8080
#define MAX_QUEUE_LENGTH 3

void read_and_display_message(int fd)
{
    char buf[1024] = {0};
    int ret = read(fd, buf, 1024);

    while (ret != FULL_READ) {
        if (ret == CLASSIC_ERROR) {
            perror("Unable to read message");
            return;
        }
        write(1, buf, ret);
        ret = read(fd, buf, 1024);
        printf("stuck here\n");
    }
}

int main(int ac, char **av)
{
    struct sockaddr_in servaddr;
    socklen_t addrlen = sizeof(servaddr);
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == CLASSIC_ERROR) {
        perror("Socket creation failed");
        return ERROR;
    } else {
        printf("Client created successfully...\n");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (const struct sockaddr *)&servaddr, addrlen) == CLASSIC_ERROR) {
        perror("Client connect");
        return ERROR;
    } else {
        printf("Connected successfully...\n");
    }

    // read_and_display_message(fd);

    char *buffer = NULL;
    size_t len = 0;
    write(1, "Enter message: ", strlen("Enter message: "));

    while (getline(&buffer, &len, stdin) != CLASSIC_ERROR) {
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
        write(fd, buffer, strlen(buffer));
        write(1, "Enter message: ", strlen("Enter message: "));
    }

    free(buffer);
    close(fd);
    return 0;
}