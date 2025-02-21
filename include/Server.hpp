/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#ifndef SERVER_HPP_
    #define SERVER_HPP_

    #include <string>
    #include <string.h>
    #include <iostream>
    #include <arpa/inet.h>
    #include <memory>

    #include "wrapped/Socket.hpp"
    #include "wrapped/Bind.hpp"
    #include "wrapped/Listen.hpp"

    #define LISTEN_BACKLOG 10000

namespace ftp
{
    class Server {
        public:
            Server(int port, char const *homePath);
            ~Server() = default;

            int getSocketFd() const { return _serverSocket->getFd(); };
            struct sockaddr_in getAddress() const { return _address; };

        protected:
        private:
            void setServerAddress(int family, u_int16_t port, in_addr_t s_addr);

            std::unique_ptr<Socket> _serverSocket;
            struct sockaddr_in _address;
    };
} // namespace ftp

#endif /* !SERVER_HPP_ */
