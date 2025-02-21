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

int connect_data_socket(const char *response) {
    int ip1, ip2, ip3, ip4, port1, port2;
    sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
           &ip1, &ip2, &ip3, &ip4, &port1, &port2);

    char ip[16];
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    int port = port1 * 256 + port2;

    struct sockaddr_in data_addr;
    int data_fd = socket(AF_INET, SOCK_STREAM, 0);

    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    data_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(data_fd, (struct sockaddr *)&data_addr, sizeof(data_addr)) == CLASSIC_ERROR) {
        perror("Data connection failed");
        close(data_fd);
        return CLASSIC_ERROR;
    }
    return data_fd;
}

void receive_file(int data_fd, const char *filepath) {
    FILE *file = fopen(filepath, "wb");
    char buffer[1024];
    int bytes_read;

    if (!file) {
        perror("Failed to create file");
        return;
    }

    while ((bytes_read = read(data_fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
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
    write(1, "Enter command: ", strlen("Enter command: "));

    while (getline(&buffer, &len, stdin) != CLASSIC_ERROR) {
        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }
        write(fd, buffer, strlen(buffer));

        if (strncmp(buffer, "PASV", 4) == 0) {
            char response[256] = {0};
            read(fd, response, sizeof(response));
            printf("Server response: %s", response);

            int data_fd = connect_data_socket(response);
            if (data_fd != CLASSIC_ERROR) {
                printf("Data connection established\n");
                // Wait for RETR command and handle file transfer
                receive_file(data_fd, "received_file");
                close(data_fd);
            }
        }
        write(1, "Enter command: ", strlen("Enter command: "));
    }

    free(buffer);
    close(fd);
    return 0;
}