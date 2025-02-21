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

void handle_client(int new_socket_fd) {
    char buffer[1024] = {0};
    int ret = read(new_socket_fd, buffer, 1024);
    while (ret > 0) {
        write(1, buffer, ret);
        ret = read(new_socket_fd, buffer, 1024);
    }
    close(new_socket_fd);
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

void help_client(struct pollfd *fds)
{
    char buffer[BUFSIZ] = {0};
    int ret = read(fds->fd, buffer, BUFSIZ);

    if (ret == CLASSIC_ERROR) {
        perror("Help client getline");
        return;
    }
    write(1, buffer, strlen(buffer));
}

void help_clients(struct pollfd *fds, int nfds)
{
    for (int i = 1; i < nfds; i++) {
        if (fds[i].events & POLLIN) {
            help_client(&fds[i]);
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

            if (nfds < MAX_CLIENTS) {
                fds[nfds].fd = new_socket_fd;
                fds[nfds].events = POLLIN;
                nfds++;
                printf("added new fd\n");
            } else {
                printf("Max clients reached. Closing new connection...\n");
                close(new_socket_fd);
            }
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