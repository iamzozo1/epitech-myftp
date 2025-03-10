/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

namespace ftp
{
    void Server::setAddress(struct sockaddr_in &address, int family, u_int16_t port, in_addr_t s_addr)
    {
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
        return newFd;
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
        char buffer[BUFSIZ] = {0};
        std::shared_ptr<Socket> socket = client.getSocket();
        ssize_t bytes;

        bytes = socket->read(buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
            return;
        buffer[bytes - 1] = '\0';

        if (strncmp(buffer, "PASV", 4) == 0) {
            client.openDataSocket();
        } else if (strncmp(buffer, "RETR ", 5) == 0) {
            try {
                client.command(RETR, buffer);
            } catch(const std::exception& e) {
                socket->write(e.what());
            }
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            socket->write("221 Goodbye.\r\n");
        } else {
            socket->write("500 Unknown command.\r\n");
        }
    }

    void Server::handleClients()
    {
        for (unsigned int i = 1; i < _fds.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                _clients[i - 1].setPollFd(_fds[i]);
                handleClient(_clients[i - 1]);
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

    void Server::updateFdsAfterPoll(struct pollfd *fds)
    {
        for (unsigned int i = 0; i < _fds.size(); i++) {
            if (fds[i].revents & POLLIN)
                _fds[i] = fds[i];
        }
    }

} // namespace ftp
