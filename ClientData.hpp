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
    #include <fstream>
    #include <arpa/inet.h>
    #include "Socket.hpp"

namespace ftp
{
    enum CommandName {
        RETR,
    };

    class ClientData {
        public:
            ClientData(std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket = nullptr, std::shared_ptr<Socket> dataSocket = nullptr);
            ~ClientData();

            void openDataSocket(void);

            void setSocket(std::shared_ptr<Socket>);

            std::shared_ptr<Socket> getSocket() const { return _socket; }
            std::shared_ptr<Socket> getDataSocket() const { return _dataSocket; }
            std::shared_ptr<struct pollfd> getPollFd() const { return _pollfd; }

            void command(CommandName cmd, std::string buffer);

        private:
            void ClientData::sendFile(const std::string& filepath);
            
            std::shared_ptr<Socket> _socket;
            std::shared_ptr<Socket> _dataSocket;
            std::shared_ptr<struct pollfd> _pollfd;
    };
} // namespace ftp

#endif /* !CLIENTDATA_HPP_ */
