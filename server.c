#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define CLASSIC_ERROR -1
#define ERROR 84
#define PORT 8080
#define MAX_QUEUE_LENGTH 3
#define MAX_CLIENTS 100
#define FD_CLOSED -1

#define FTP_PASV_OK "227"
#define FTP_FILE_OK "150"
#define FTP_TRANSFER_COMPLETE "226"
#define FTP_ERROR "550"

int create_data_socket(int control_socket_fd) {
    struct sockaddr_in data_addr;
    int data_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (data_socket_fd == CLASSIC_ERROR) {
        perror("Data socket creation failed");
        return CLASSIC_ERROR;
    }

    memset(&data_addr, 0, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_addr.s_addr = INADDR_ANY;
    data_addr.sin_port = 0; // Let the system choose a free port

    if (bind(data_socket_fd, (struct sockaddr *)&data_addr, sizeof(data_addr)) == CLASSIC_ERROR) {
        perror("Data socket bind failed");
        close(data_socket_fd);
        return CLASSIC_ERROR;
    }

    printf("before listening to data socket\n");
    if (listen(data_socket_fd, MAX_QUEUE_LENGTH) == CLASSIC_ERROR) {
        perror("Data socket listen failed");
        close(data_socket_fd);
        return CLASSIC_ERROR;
    }

    struct sockaddr_in bound_addr;
    socklen_t addr_len = sizeof(bound_addr);
    printf("before getting sockname\n");
    if (getsockname(data_socket_fd, (struct sockaddr *)&bound_addr, &addr_len) == CLASSIC_ERROR) {
        perror("Data socket getsockname failed");
        close(data_socket_fd);
        return CLASSIC_ERROR;
    }

    int port = ntohs(bound_addr.sin_port);
    char response[256];
    snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n", port / 256, port % 256);
    printf("writing response\n");
    write(control_socket_fd, response, strlen(response));

    return data_socket_fd;
}

void send_file(int control_fd, int data_fd, const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    char buffer[1024];
    size_t bytes_read;

    if (!file) {
        write(control_fd, FTP_ERROR " Failed to open file\r\n", strlen(FTP_ERROR) + 20);
        return;
    }

    write(control_fd, FTP_FILE_OK " File status okay\r\n", strlen(FTP_FILE_OK) + 19);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(data_fd, buffer, bytes_read);
    }

    fclose(file);
    close(data_fd);
    write(control_fd, FTP_TRANSFER_COMPLETE " Transfer complete\r\n",
          strlen(FTP_TRANSFER_COMPLETE) + 19);
}

void handle_client(struct pollfd *fds) {
    int control_fd = fds->fd;
    char buffer[1024] = {0};
    int ret;
    int data_fd = CLASSIC_ERROR;

    while ((ret = read(control_fd, buffer, 1024)) > 0) {
        if (strncmp(buffer, "PASV", 4) == 0) {
            data_fd = create_data_socket(control_fd);
        } else if (strncmp(buffer, "RETR ", 5) == 0) {
            if (data_fd == -1) {
                write(control_fd, FTP_ERROR " Use PASV first\r\n", strlen(FTP_ERROR) + 16);
                continue;
            }
            char filepath[1024];
            sscanf(buffer, "RETR %s", filepath);

            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_data_fd = accept(data_fd, (struct sockaddr *)&client_addr, &addr_len);

            if (client_data_fd != CLASSIC_ERROR) {
                send_file(control_fd, client_data_fd, filepath);
                close(data_fd);
                data_fd = CLASSIC_ERROR;
            }
        }
        memset(buffer, 0, sizeof(buffer));
        printf("waiting for new command\n");
    }
    if (data_fd != CLASSIC_ERROR)
        close(data_fd);
}

int setup_server_socket() {
    struct sockaddr_in servaddr;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == CLASSIC_ERROR) {
        perror("Socket creation failed");
        return CLASSIC_ERROR;
    } else {
        printf("Server created successfully...\n");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == CLASSIC_ERROR) {
        perror("Unable to assign name to socket");
        close(fd);
        return CLASSIC_ERROR;
    } else {
        printf("Name assigned to socket...\n");
    }

    if (listen(fd, MAX_QUEUE_LENGTH) == CLASSIC_ERROR) {
        perror("Server error on listen");
        close(fd);
        return CLASSIC_ERROR;
    } else {
        printf("Server listening...\n");
    }

    return fd;
}

void help_clients(struct pollfd *fds, int nfds)
{
    for (int i = 1; i < nfds; i++) {
        if (fds[i].events & POLLIN) {
            handle_client(&fds[i]);
        }
    }
}

void handle_connections(int server_fd) {
    struct sockaddr_in clientaddr;
    socklen_t client_addrlen = sizeof(clientaddr);
    struct pollfd fds[MAX_CLIENTS];
    int nfds = 1;
    char *msg = "Hello world !!!\n";

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        printf("before poll\n");
        int poll_ret = poll(fds, nfds, -1);
        printf("after poll\n");

        if (poll_ret == CLASSIC_ERROR) {
            perror("Poll error");
            close(server_fd);
            exit(ERROR);
        }

        if (fds[0].revents & POLLIN) {
            int new_socket_fd = accept(server_fd, (struct sockaddr *)&clientaddr, &client_addrlen);

            if (new_socket_fd == CLASSIC_ERROR) {
                perror("Server error on accept");
                continue;
            }

            printf("New connection accepted...\n");
        } else {
            printf("before help clients\n");
            help_clients(fds, nfds);
            printf("after help clients\n");
        }

        // Remove closed connections
        for (unsigned int i = 1; i < nfds; i++) {
            if (fds[i].fd == FD_CLOSED) {
                for (unsigned int j = i; j < nfds - 1; j++) {
                    fds[j] = fds[j + 1];
                }
                nfds--;
                i--;
            }
        }
    }
}

int main() {
    int server_fd = setup_server_socket();
    if (server_fd == CLASSIC_ERROR) {
        return ERROR;
    }

    handle_connections(server_fd);

    close(server_fd);
    return 0;
}