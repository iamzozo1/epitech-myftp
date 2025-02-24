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
    #include <vector>

    #include "wrapped/Socket.hpp"
    #include "wrapped/Poll.hpp"

    #define LISTEN_BACKLOG 10000

namespace ftp
{
    class Server {
        public:
            Server(int port, char const *homePath);
            ~Server() = default;

            std::vector<struct pollfd> getFds() const { return _fds; }

            void addFdToServer(int fd);
            void connectClient();
            void handleClients();

        protected:
        private:
            void setAddress(struct sockaddr_in, int family, u_int16_t port, in_addr_t s_addr) const;
            void handleClient(struct pollfd &client);
            int openDataSocket(int clientFd) const;

            std::shared_ptr<Socket> _serverSocket;
            struct sockaddr_in _address;
            std::vector<struct pollfd> _fds;
    };
} // namespace ftp

#endif /* !SERVER_HPP_ */
