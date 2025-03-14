/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

namespace ftp
{
    std::string Server::getClientCommand(std::string buffer) const
    {
        std::string cmd;
        size_t pos = buffer.find_first_of(" \r\n\0\t");

        if (buffer.empty())
            return buffer;
        if (pos != std::string::npos) {
            cmd = buffer.substr(0, pos);
        } else {
            cmd = buffer;
        }
        return cmd;
    }

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
        ClientData newClient = ClientData(_homePath, std::make_shared<struct pollfd>(newPollFd), newClientSocket, nullptr);

        _clients.push_back(newClient);

        newClient.getSocket()->write("220 Service ready for new user.");
    }

    void Server::closeClientConnection(unsigned int clientNb)
    {
        _clients.erase(_clients.begin() + (clientNb - 1));
        _fds.erase(_fds.begin() + clientNb);
    }

    void Server::handleClient(ClientData &client)
    {
        char buffer[BUFSIZ] = {0};
        std::shared_ptr<Socket> socket = client.getSocket();
        ssize_t bytes;
        std::string cmd;

        bytes = socket->read(buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
            return;
        buffer[bytes - 1] = '\0';
        cmd = getClientCommand(buffer);
        std::cerr << buffer << std::endl;

        try {
            client.command(cmd, buffer);
        } catch (const ConnectionClosed &e) {
            socket->write(e.what());
            throw ConnectionClosed();
        } catch (const std::exception &e) {
            socket->write(e.what());
        }
    }

    void Server::handleClients()
    {
        for (unsigned int i = 1; i < _fds.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                _clients[i - 1].setPollFd(_fds[i]);
                try {
                    handleClient(_clients[i - 1]);
                    _fds[i].revents = 0;
                } catch (const ConnectionClosed &e) {
                    closeClientConnection(i);
                } catch (const ReadError &e) {
                    closeClientConnection(i);
                }
            }
        }
    }

    void Server::updateFdsAfterPoll(struct pollfd *fds)
    {
        for (unsigned int i = 0; i < _fds.size(); i++) {
            if (fds[i].revents & POLLIN)
                _fds[i] = fds[i];
        }
    }

    Server::Server(int port, char const *homePath) : _serverSocket(nullptr)
    {
        _homePath = homePath;
        _serverSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
        setAddress(_address, AF_INET, port, INADDR_ANY);
        _serverSocket->setSockAddress((struct sockaddr *)&_address, (socklen_t)sizeof(_address));
        _serverSocket->bind();
        _serverSocket->listen(LISTEN_BACKLOG);
        addFdToServer(_serverSocket->_fd);
    }

} // namespace ftp
