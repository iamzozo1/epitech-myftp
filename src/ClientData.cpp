/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** ClientData
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "ClientData.hpp"
#include "Server.hpp"

namespace ftp
{
    ClientData::ClientData(std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket, std::shared_ptr<Socket> dataSocket) : _socket(socket), _dataSocket(dataSocket), _pollfd(pollfd), _user(""), _path("/")
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
        char response[BUFSIZ];
        snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)", port / 256, port % 256);
        _socket->write(response);
    }

    void ClientData::sendFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        ssize_t bytesRead = 0;
        char buffer[BUFSIZ];

        if (!file.is_open()) {
            throw FileOpenError();
        }
        _socket->write("150 File status okay; about to open data connection.");

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
        _socket->write("Closing data connection.");
    }

    std::string ClientData::getCommandArg(std::string buffer) const
    {
        size_t pos = buffer.find_last_of(" \t", COMMAND_SIZE);

        if (pos == std::string::npos || pos + 1 >= buffer.size())
            throw InvalidCommandError();
        return buffer.substr(pos + 1);
    }

    std::string ClientData::getNewPath(std::string buffer) const
    {
        std::string newPath;

        if (buffer[0] == '/') {
            newPath = buffer;
        } else if (buffer.compare("..") == 0) {
            size_t lastSlash = _path.find_last_of('/');
            if (lastSlash != std::string::npos && _path != "/") {
                newPath = _path.substr(0, lastSlash);
                if (newPath.empty())
                    newPath = "/";
            } else {
                newPath = _path;
            }
        } else if (buffer.compare(".") == 0) {
            return _path;
        } else {
            if (_path.compare("/") == 0)
                newPath = _path + buffer;
            else
                newPath = _path + "/" + buffer;
        }
        return newPath;
    }

    bool ClientData::changeWorkingDirectory(std::string arg)
    {
        std::string newPath = getNewPath(arg);

        if (chdir(newPath.c_str()) != ERROR) {
            _path = newPath;
            return true;
        }
        _socket->write("550 Failed to change directory.");
        return false;
    }

    void ClientData::listDir(const std::string& path) const
    {
        struct dirent *entry;
        DIR *dir = opendir(path.empty() ? _path.c_str() : path.c_str());

        if (dir == NULL) {
            _socket->write("550 Failed to open directory.");
            return;
        }

        _socket->write("150 Here comes the directory listing.");
        _dataSocket->accept(NULL, NULL);

        while ((entry = readdir(dir)) != NULL) {
            std::string filename = entry->d_name;
            struct stat filestat;
            std::string fullpath = path.empty() ? _path + "/" + filename : path + "/" + filename;

            if (stat(fullpath.c_str(), &filestat) == 0) {
                char perms[11] = {0};
                char tstring[80] = {0};
                struct tm *time = localtime(&filestat.st_mtime);

                perms[0] = S_ISDIR(filestat.st_mode) ? 'd' : '-';
                perms[1] = (filestat.st_mode & S_IRUSR) ? 'r' : '-';
                perms[2] = (filestat.st_mode & S_IWUSR) ? 'w' : '-';
                perms[3] = (filestat.st_mode & S_IXUSR) ? 'x' : '-';
                perms[4] = (filestat.st_mode & S_IRGRP) ? 'r' : '-';
                perms[5] = (filestat.st_mode & S_IWGRP) ? 'w' : '-';
                perms[6] = (filestat.st_mode & S_IXGRP) ? 'x' : '-';
                perms[7] = (filestat.st_mode & S_IROTH) ? 'r' : '-';
                perms[8] = (filestat.st_mode & S_IWOTH) ? 'w' : '-';
                perms[9] = (filestat.st_mode & S_IXOTH) ? 'x' : '-';

                // Format time
                strftime(tstring, sizeof(tstring), "%b %d %H:%M", time);

                char buffer[BUFSIZ];
                snprintf(buffer, sizeof(buffer), "%s 1 user group %8ld %s %s\r\n",
                    perms, (long)filestat.st_size, tstring, entry->d_name);
                _dataSocket->write(buffer);
            }
        }
        closedir(dir);
        _socket->write("150 File status okay; about to open data connection.");
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
        } else if (cmd == PASS) {
            if (_user.empty())
                _socket->write("332 Need account for login.");
            else {
                try {
                    _password = getCommandArg(buffer);
                } catch(const InvalidCommandError &e) {
                    _password = "";
                }
                _socket->write("230 User logged in, proceed.");
            }
        } else if (cmd == CWD) {
            if (changeWorkingDirectory(getCommandArg(buffer))) {
                _socket->write("250 Requested file action okay, completed.");
            }
        } else if (cmd == CDUP) {
            if (changeWorkingDirectory("..")) {
                _socket->write("200 Command okay.");
            }
        } else if (cmd == PWD) {
            _socket->write(_path);
        } else if (cmd == QUIT) {
            throw ConnectionClosed();
        } else if (cmd == PASV) {
            openDataSocket();
        // } else if (cmd == LIST) {
        //     try {
        //         arg = getCommandArg(buffer);
        //     } catch(const InvalidCommandError &e) {
        //         arg = ".";
        //     }
        //     listDir(getNewPath(arg));
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
