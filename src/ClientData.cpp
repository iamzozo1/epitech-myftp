/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** ClientData
*/

#include "ClientData.hpp"
#include "Server.hpp"

namespace ftp
{
    ClientData::ClientData(std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket, std::shared_ptr<Socket> dataSocket) : _socket(socket), _dataSocket(dataSocket), _pollfd(pollfd), _user("")
    {
    }

    void ClientData::setSocket(std::shared_ptr<Socket> socket)
    {
        if (socket == nullptr)
            return;
        _socket = socket;
    }

    void ClientData::setPollFd(struct pollfd &p)
    {
        _pollfd = std::make_shared<struct pollfd>(p);
    }

    void ClientData::openDataSocket(void)
    {
        if (_socket == nullptr)
            throw Error("No control connection");

        _dataSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in data_addr;
        memset(&data_addr, 0, sizeof(data_addr));

        Server::setAddress(data_addr, AF_INET, 0, INADDR_ANY);
        _dataSocket->setSockAddress((struct sockaddr *)&data_addr, sizeof(data_addr));
        _dataSocket->bind();
        _dataSocket->listen(1);
        _dataSocket->getSockName();

        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        if (getsockname(_dataSocket->_fd, (struct sockaddr*)&addr, &len) == ERROR)
            throw Error("getsockname failed");

        int port = ntohs(addr.sin_port);
        char response[256];
        snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n", port / 256, port % 256);
        _socket->write(response);
    }

    void ClientData::sendFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        ssize_t bytesRead = 0;
        char buffer[1024];

        if (!file.is_open()) {
            throw FileOpenError();
        }
        _socket->write("150 File status okay\r\n");

        while (file) {
            file.read(buffer, sizeof(buffer));
            bytesRead = file.gcount();
            if (bytesRead > 0) {
                if (_dataSocket->write(buffer) <= 0) {
                    throw DataSocketWriteError();
                }
            }
        }
        file.close();
        _socket->write("226 Transfer complete. Closing data connection\r\n");
    }

    std::string ClientData::getCommandArg(std::string buffer) const
    {
        size_t pos = buffer.find_last_of(" \t", 4);

        if (pos == std::string::npos || pos + 1 >= buffer.size())
            throw InvalidCommandError();
        return buffer.substr(pos + 1);
    }

    void ClientData::command(CommandName cmd, std::string buffer)
    {
        char filepath[BUFSIZ];
        char readBuffer[BUFSIZ] = {0};
        std::string arg;

        if (cmd == RETR) {
            if (_dataSocket == nullptr) {
                throw DataSocketNullError();
            }
            sscanf(buffer.c_str(), "RETR %s", filepath);
            _dataSocket->accept(NULL, NULL);
            try {
                sendFile(filepath);
            } catch(const std::exception& e) {
                _socket->write(e.what());
            }
            _dataSocket.reset();
        } else if (cmd == USER) {
            _user = getCommandArg(buffer);
            _socket->write("331 User name okay, need password.");
        } else {
            throw InvalidCommandError();
        }
    }

    void ClientData::setPollFdAsRead()
    {
        _pollfd->revents = 0;
    }

    void ClientData::setUser(std::string user)
    {
        _user = user;
    }

} // namespace ftp
