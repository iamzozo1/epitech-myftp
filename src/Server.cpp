/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

namespace ftp
{
    void Server::setAddress(struct sockaddr_in address, int family, u_int16_t port, in_addr_t s_addr) const
    {
        address = {0};
        address.sin_family = family;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = s_addr;
    }

    void Server::addFdToServer(int fd)
    {
        struct pollfd newFd = {0};

        newFd.fd = fd;
        newFd.events = POLLIN;
        _fds.push_back(newFd);
    }

    void Server::connectClient()
    {
        struct sockaddr_in clientaddr;
        socklen_t client_addrlen = sizeof(clientaddr);

        Accept a = Accept(_serverSocket->getFd(), (struct sockaddr *)&clientaddr, &client_addrlen);
        addFdToServer(a.getRet());
    }

    int Server::openDataSocket(int clientFd) const
    {
        struct sockaddr_in data_addr;
        struct sockaddr_in bound_addr;
        socklen_t addr_len = sizeof(bound_addr);
        Socket dataSocket = Socket(AF_INET, SOCK_STREAM, 0);

        setAddress(data_addr, AF_INET, INADDR_ANY, 0);
        Bind(dataSocket.getFd(), (struct sockaddr *)&data_addr, sizeof(data_addr));
        Listen(dataSocket.getFd(), LISTEN_BACKLOG);

        dataSocket.getSockName((struct sockaddr *)&bound_addr, &addr_len);

        int port = ntohs(bound_addr.sin_port);
        char response[256];
        snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n", port / 256, port % 256);
        printf("writing response\n");
        write(control_socket_fd, response, strlen(response));

        return data_socket_fd;
    }

    void Server::handleClient(struct pollfd &client)
    {
        char buffer[1024] = {0};
        int ret;
        int data_fd = -1;

        while ((ret = read(client.fd, buffer, 1024)) > 0) {
            if (strncmp(buffer, "PASV", 4) == 0) {
                data_fd = create_data_socket(client.fd);
            } else if (strncmp(buffer, "RETR ", 5) == 0) {
                if (data_fd == -1) {
                    write(client.fd, FTP_ERROR " Use PASV first\r\n", strlen(FTP_ERROR) + 16);
                    continue;
                }
                char filepath[1024];
                sscanf(buffer, "RETR %s", filepath);

                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int client_data_fd = accept(data_fd, (struct sockaddr *)&client_addr, &addr_len);

                if (client_data_fd != -1) {
                    send_file(client.fd, client_data_fd, filepath);
                    close(data_fd);
                    data_fd = -1;
                }
            }
            memset(buffer, 0, sizeof(buffer));
            printf("waiting for new command\n");
        }
        if (data_fd != -1)
            close(data_fd);
    }

    void Server::handleClients()
    {
        for (unsigned int i = 0; i < _fds.size(); i++) {
            if (_fds[i].events & POLLIN)
                handleClient(_fds[i]);
        }
    }

    Server::Server(int port, char const *homePath) : _serverSocket(nullptr)
    {
        (void)homePath;
        _serverSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
        setAddress(_address, AF_INET, port, INADDR_ANY);
        _serverSocket->setSockAddress((struct sockaddr *)&_address, (socklen_t)sizeof(_address));
        _serverSocket->bind();
        _serverSocket->listen(LISTEN_BACKLOG);
        addFdToServer(_serverSocket->_fd);
    }
} // namespace ftp

