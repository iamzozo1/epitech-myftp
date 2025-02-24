/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

namespace ftp
{
    void Server::setAddress(struct sockaddr_in address, int family, u_int16_t port, in_addr_t s_addr)
    {
        address = {0};
        address.sin_family = family;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = s_addr;
    }

    struct pollfd Server::addFdToServer(int fd)
    {
        struct pollfd newFd = {0};

        newFd.fd = fd;
        newFd.events = POLLIN;
        _fds.push_back(newFd);
    }

    void Server::connectClient()
    {
        int newClientFd = _serverSocket->accept(NULL, NULL);
        std::shared_ptr<Socket> newClientSocket = std::make_shared<Socket>(newClientFd);
        struct pollfd newPollFd = addFdToServer(newClientSocket->_fd);
        ClientData newClient = ClientData(std::make_shared<struct pollfd>(newPollFd), newClientSocket, nullptr);

        _clients.push_back(newClient);
    }

    void Server::handleClient(ClientData &client)
    {
        char buffer[1024] = {0};
        int ret;
        int clientFd = client.getSocket()->_fd;
        std::shared_ptr<Socket> dataSocket = client.getDataSocket();

        while ((ret = read(clientFd, buffer, 1024)) > 0) {
            if (strncmp(buffer, "PASV", 4) == 0) {
                client.openDataSocket();
                dataSocket = client.getDataSocket();
            } else if (strncmp(buffer, "RETR ", 5) == 0) {
                client.command(RETR, buffer);
            }
            memset(buffer, 0, sizeof(buffer));
        }
    }

    void Server::handleClients()
    {
        for (auto &client : _clients) {
            if (client.getPollFd()->events & POLLIN) {
                handleClient(client);
            }
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

