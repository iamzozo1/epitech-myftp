/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** ClientData
*/

#ifndef CLIENTDATA_HPP_
    #define CLIENTDATA_HPP_

    #include <memory>
    #include <string>
    #include <poll.h>
    #include <fstream>
    #include <functional>
    #include <iostream>
    #include <arpa/inet.h>
    #include <stdio.h>
    #include "wrapped/Socket.hpp"

    #define PERMISSION_NUMBER 10
    #define TIME_STRING_SIZE 80
    #define END_OF_READ 0

namespace ftp
{
    enum CommandName {
        USER,
        PASS,
        CWD,
        CDUP,
        QUIT,
        PORT,
        PASV,
        STOR,
        RETR,
        LIST,
        DELE,
        PWD,
        HELP,
        NOOP,
        SYST,
        TYPE,
        UNKNOWN
    };

    class ClientData {
        public:
            ClientData(std::string homepath, std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket = nullptr, std::shared_ptr<Socket> dataSocket = nullptr);
            ~ClientData() = default;

            void setSocket(std::shared_ptr<Socket>);
            void setPollFd(struct pollfd &s);
            void setPollFdAsRead();
            void setUser(std::string user);

            std::shared_ptr<Socket> getSocket() const { return _socket; }
            std::shared_ptr<Socket> getDataSocket() const { return _dataSocket; }
            std::shared_ptr<struct pollfd> getPollFd() const { return _pollfd; }
            std::string getUser() const { return _user; }

            void command(std::string cmd, std::string buffer);

        private:
            void openDataSocket(std::string buffer);
            void sendFile(const std::string& filepath);
            void changeWorkingDirectory(std::string buffer);
            void changeWorkingDirectoryUp(std::string buffer);
            void listDir(std::string buffer);
            std::string getCommandArg(std::string buffer) const;
            std::string getNewPath(std::string buffer) const;
            void closeDataSocket();
            void downloadFile(std::string buffer);
            void writeNewFile(std::string filename);
            void deleteFile(std::string filename);
            void connectToPort(std::string buffer);
            void connectUser(std::string buffer);
            void enterUserPassword(std::string buffer);
            void chdir(std::string newPath);
            void pwd(std::string buffer);
            void quit(std::string buffer);
            void syst(std::string buffer);
            void successCommand(std::string buffer);

            std::shared_ptr<Socket> _socket;
            std::shared_ptr<Socket> _dataSocket;
            std::shared_ptr<struct pollfd> _pollfd;
            std::string _user;
            std::string _password;
            std::string _path;
    };
} // namespace ftp

#endif /* !CLIENTDATA_HPP_ */
