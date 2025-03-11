/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

namespace ftp
{
    CommandName Server::getClientCommand(std::string buffer) const
    {
        if (buffer.empty() || buffer.size() < COMMAND_SIZE)
            return UNKNOWN;

        std::string cmd = buffer.substr(0, 4);
        std::unordered_map<std::string, CommandName> commandsMap;

        commandsMap["PASV"] = PASV;
        commandsMap["USER"] = USER;
        commandsMap["PASS"] = PASS;
        commandsMap["CWD "] = CWD;
        commandsMap["CDUP"] = CDUP;
        commandsMap["QUIT"] = QUIT;
        commandsMap["PORT"] = PORT;
        commandsMap["PASV"] = PASV;
        commandsMap["STOR"] = STOR;
        commandsMap["RETR"] = RETR;
        commandsMap["LIST"] = LIST;
        commandsMap["DELE"] = DELE;
        commandsMap["PWD "] = PWD;
        commandsMap["HELP"] = HELP;
        commandsMap["NOOP"] = NOOP;

        auto it = commandsMap.find(cmd);
        if (it != commandsMap.end())
            return it->second;
        return UNKNOWN;
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
        ClientData newClient = ClientData(std::make_shared<struct pollfd>(newPollFd), newClientSocket, nullptr);

        _clients.push_back(newClient);
        newClient.getSocket()->write("220 Connected successfully to Enzo's FTP Server.\r\n");
    }

    void Server::handleClient(ClientData &client)
    {
        char buffer[BUFSIZ] = {0};
        std::shared_ptr<Socket> socket = client.getSocket();
        ssize_t bytes;
        CommandName cmd;

        bytes = socket->read(buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
            return;
        buffer[bytes - 1] = '\0';
        cmd = getClientCommand(buffer);

        if (cmd == UNKNOWN) {
            socket->write("500 Unknown command.\r\n");
        } else if (cmd == PASV) {
            client.openDataSocket();
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            socket->write("221 Goodbye.\r\n");
        } else {
            try {
                client.command(cmd, buffer);
            } catch(const std::exception& e) {
                socket->write(e.what());
            }
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
