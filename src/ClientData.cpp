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
    ClientData::ClientData(std::string homepath, std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket, std::shared_ptr<Socket> dataSocket) : _socket(socket), _dataSocket(dataSocket), _pollfd(pollfd), _homePath(homepath), _path(homepath), _user(""), _passiveMode(true), _authSucceeded(false)
    {}

    void ClientData::closeDataSocket()
    {
        _socket->write("226 Closing data connection.");
        _dataSocket.reset();
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

    void ClientData::openDataSocket(std::string buffer)
    {
        (void)buffer;
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
        if (getsockname(_dataSocket->_fd, (struct sockaddr *)&addr, &len) == ERROR)
            throw Error("getsockname failed");

        int port = ntohs(addr.sin_port);
        char response[BUFSIZ];
        snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)", port / 256, port % 256);
        _socket->write(response);
    }

    void ClientData::sendFile(const std::string &filepath)
    {
        std::shared_ptr<Socket> transferSocket = _dataSocket;
        std::ifstream file(filepath, std::ios::binary);
        ssize_t bytesRead = 0;
        char buffer[BUFSIZ];

        if (!file.is_open()) {
            throw FileOpenError();
        }
        _socket->write("150 File status okay; about to open data connection.");
        if (_passiveMode) {
            transferSocket = std::make_shared<Socket>(_dataSocket->accept(NULL, NULL));
        } else {
            _passiveMode = true;
        }

        while (file) {
            file.read(buffer, sizeof(buffer));
            bytesRead = file.gcount();
            if (bytesRead > 0)
            {
                if (_dataSocket->write(buffer) <= 0)
                {
                    throw DataSocketWriteError();
                }
            }
        }
        file.close();
    }

    std::string ClientData::getCommandArg(std::string buffer) const
    {
        size_t pos = buffer.find_last_of(" \t", COMMAND_SIZE);

        if (pos == std::string::npos || pos + 1 >= buffer.size())
        {
            throw InvalidCommandError();
        }
        std::string arg = buffer.substr(pos + 1);

        while (!arg.empty() && (arg.back() == '\r' || arg.back() == '\n' || arg.back() == ' ' || arg.back() == '\t'))
        {
            arg.pop_back();
        }
        return arg;
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
        } else
        {
            if (_path.compare("/") == 0)
                newPath = _path + buffer;
            else
                newPath = _path + "/" + buffer;
        }
        if (newPath.find(_homePath) != 0) {
            throw InvalidPathError();
        }
        return newPath;
    }

    void ClientData::changeWorkingDirectory(std::string buffer)
    {
        std::string newPath = getNewPath(getCommandArg(buffer));

        try {
            chdir(newPath);
            _socket->write("250 Requested file action okay, completed.");
        } catch (const ChdirError &e) {
            _socket->write("550 Failed to change directory.");
        }
    }

    void ClientData::changeWorkingDirectoryUp(std::string buffer)
    {
        std::string newPath = getNewPath("..");

        try {
            chdir(newPath);
            _socket->write("200 Command okay.");
        } catch (const ChdirError &e) {
            _socket->write("550 Failed to change directory.");
        }
    }

    void ClientData::chdir(std::string newPath)
    {
        if (::chdir(newPath.c_str()) == ERROR) {
            throw ChdirError();
        }
        _path = newPath;
    }

    void ClientData::listDir(std::string buffer)
    {
        std::string arg;
        std::string path;

        try {
            arg = getCommandArg(buffer);
        } catch (const InvalidCommandError &e) {
            arg = ".";
        }
        path = getNewPath(arg);
        if (_dataSocket == nullptr) {
            throw DataSocketNullError();
        }
        struct dirent *el;
        DIR *dir = opendir(path.empty() ? _path.c_str() : path.c_str());

        if (dir == NULL) {
            _socket->write("550 Failed to open directory.");
            return;
        }
        _socket->write("150 File status okay; about to open data connection.");

        std::shared_ptr<Socket> transferSocket = _dataSocket;

        if (_passiveMode) {
            transferSocket = std::make_shared<Socket>(_dataSocket->accept(NULL, NULL));
        } else {
            _passiveMode = true;
        }

        std::string listing;
        while ((el = readdir(dir)) != NULL) {
            std::string filename = el->d_name;
            struct stat filestat;
            std::string fullpath = path.empty() ? _path + "/" + filename : path + "/" + filename;

            if (stat(fullpath.c_str(), &filestat) == 0) {
                char perms[PERMISSION_NUMBER + 1] = {0};
                char tstring[TIME_STRING_SIZE] = {0};
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

                strftime(tstring, sizeof(tstring), "%b %d %H:%M", time);

                char buffer[BUFSIZ];
                snprintf(buffer, sizeof(buffer), "%s 1 user group %8ld %s %s\r\n", perms, (long)filestat.st_size, tstring, el->d_name);
                listing += buffer;
            }
        }
        transferSocket->write(listing);
        closeDataSocket();
        closedir(dir);
    }

    void ClientData::downloadFile(std::string buffer)
    {
        char filepath[BUFSIZ];

        if (_dataSocket == nullptr) {
            throw DataSocketNullError();
        }
        sscanf(buffer.c_str(), "RETR %s", filepath);

        try {
            sendFile(getNewPath(filepath));
        } catch(const std::exception& e) {
            _socket->write(e.what());
        }
        closeDataSocket();
    }

    void ClientData::writeNewFile(std::string buffer)
    {
        std::shared_ptr<Socket> transferSocket = _dataSocket;
        std::ofstream file(getNewPath(getCommandArg(buffer)));
        char fileContent[BUFSIZ];

        _socket->write("150 File status okay; about to open data connection.");
        if (_passiveMode) {
            transferSocket = std::make_shared<Socket>(_dataSocket->accept(NULL, NULL));
        } else {
            _passiveMode = true;
        }

        while (transferSocket->read(fileContent, sizeof(fileContent)) != END_OF_READ)
            file << fileContent;

        file.close();
        closeDataSocket();
    }

    void ClientData::deleteFile(std::string buffer)
    {
        try {
            if (remove(getNewPath(getCommandArg(buffer)).c_str()) == ERROR)
                throw RemoveError();
            _socket->write("250 Requested file action okay, completed.");
        } catch (const RemoveError &e) {
            std::cerr << e.what() << '\n';
            _socket->write("550 Failed to remove file.");
        }
    }

    void ClientData::connectToPort(std::string buffer)
    {
        std::string address = getCommandArg(buffer);
        int h1, h2, h3, h4, p1, p2;

        if (sscanf(address.c_str(), "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
            _socket->write("501 Syntax error in parameters or arguments.");
            return;
        }

        std::string ipAddress = std::to_string(h1) + "." + std::to_string(h2) + "." + std::to_string(h3) + "." + std::to_string(h4);
        int port = p1 * 256 + p2;

        if (_dataSocket != nullptr) {
            _dataSocket.reset();
        }

        _dataSocket = std::make_shared<Socket>(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        Server::setAddress(client_addr, AF_INET, port, inet_addr(ipAddress.c_str()));
        _dataSocket->setSockAddress((struct sockaddr *)&client_addr, sizeof(client_addr));
        _dataSocket->connect();
        _passiveMode = false;
        _socket->write("200 PORT command successful.");
    }

    void ClientData::connectUser(std::string buffer)
    {
        _user = getCommandArg(buffer);
        _socket->write("331 User name okay, need password.");
    }

    void ClientData::enterUserPassword(std::string buffer)
    {
        if (_user.empty())
            _socket->write("332 Need account for login.");
        else {
            try {
                _password = getCommandArg(buffer);
            } catch (const InvalidCommandError &e) {
                _password = "";
            }
            if (_user.compare(DEFAULT_USER) == 0 && _password.empty()) {
                _socket->write("230 User logged in, proceed.");
                _authSucceeded = true;
            } else
                _socket->write("530 nop, this is not the good password");
        }
    }

    void ClientData::pwd(std::string buffer)
    {
        (void)buffer;
        _socket->write(_path);
    }

    void ClientData::quit(std::string buffer)
    {
        (void)buffer;
        throw ConnectionClosed();
    }

    void ClientData::syst(std::string buffer)
    {
        (void)buffer;
        _socket->write("215 UNIX Type: L8");
    }

    void ClientData::successCommand(std::string buffer)
    {
        (void)buffer;
        _socket->write("200 Command okay.");
    }

    void ClientData::help(std::string buffer)
    {
        (void)buffer;
        _socket->write("214 Help message.");
        _socket->write("PASS: Specify password for authentication.");
        _socket->write("CWD: Change working directory.");
        _socket->write("CDUP: Change to parent directory.");
        _socket->write("QUIT: Terminate the connection.");
        _socket->write("PORT: Specify an address and port to which the server should connect.");
        _socket->write("PASV: Enter passive mode.");
        _socket->write("STOR: Store a file on the server.");
        _socket->write("RETR: Retrieve a file from the server.");
        _socket->write("LIST: List files in the current directory.");
        _socket->write("DELE: Delete a file on the server.");
        _socket->write("PWD: Print the current directory.");
        _socket->write("HELP: Display help information.");
        _socket->write("NOOP: No operation (used to keep connection alive).");
        _socket->write("SYST: Return system type.");
        _socket->write("TYPE: Specify data transfer type.");
        _socket->write("USER: Specify user for authentication.");
    }

    void ClientData::command(std::string cmd, std::string buffer)
    {
        std::unordered_map<std::string, std::function<void(std::string)>> commandMap;

        if (!_authSucceeded && cmd.compare("USER") != 0 && cmd.compare("PASS") != 0)
            throw NotLoggedInError();
        commandMap["USER"] = [this](std::string buffer) { connectUser(buffer); };
        commandMap["PASS"] = [this](std::string buffer) { enterUserPassword(buffer); };
        commandMap["CWD"] = [this](std::string buffer) { changeWorkingDirectory(buffer); };
        commandMap["CDUP"] = [this](std::string buffer) { changeWorkingDirectoryUp(buffer); };
        commandMap["QUIT"] = [this](std::string buffer) { quit(buffer); };
        commandMap["PORT"] = [this](std::string buffer) { connectToPort(buffer); };
        commandMap["PASV"] = [this](std::string buffer) { openDataSocket(buffer); };
        commandMap["STOR"] = [this](std::string buffer) { writeNewFile(buffer); };
        commandMap["RETR"] = [this](std::string buffer) { downloadFile(buffer); };
        commandMap["LIST"] = [this](std::string buffer) { listDir(buffer); };
        commandMap["DELE"] = [this](std::string buffer) { deleteFile(buffer); };
        commandMap["PWD"] = [this](std::string buffer) { pwd(buffer); };
        commandMap["HELP"] = [this](std::string buffer) { help(buffer); };
        commandMap["NOOP"] = [this](std::string buffer) { successCommand(buffer); };
        commandMap["SYST"] = [this](std::string buffer) { syst(buffer); };
        commandMap["TYPE"] = [this](std::string buffer) { successCommand(buffer); };

        auto it = commandMap.find(cmd);

        if (it != commandMap.end()) {
            commandMap[cmd](buffer);
            return;
        }
        throw InvalidCommandError();
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
