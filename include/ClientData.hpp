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
    #include <arpa/inet.h>
    #include "wrapped/Socket.hpp"

namespace ftp
{
    enum CommandName {
        RETR,
    };

    class ClientData {
        public:
            ClientData(std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket = nullptr, std::shared_ptr<Socket> dataSocket = nullptr);
            ~ClientData() = default;

            void openDataSocket(void);

            void setSocket(std::shared_ptr<Socket>);
            void setPollFd(struct pollfd &s);

            std::shared_ptr<Socket> getSocket() const { return _socket; }
            std::shared_ptr<Socket> getDataSocket() const { return _dataSocket; }
            std::shared_ptr<struct pollfd> getPollFd() const { return _pollfd; }
            void setPollFdAsRead();

            void command(CommandName cmd, std::string buffer);

        private:
            void sendFile(const std::string& filepath);

            std::shared_ptr<Socket> _socket;
            std::shared_ptr<Socket> _dataSocket;
            std::shared_ptr<struct pollfd> _pollfd;
    };
} // namespace ftp

#endif /* !CLIENTDATA_HPP_ */
