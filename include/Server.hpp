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
    #include <poll.h>
    #include <arpa/inet.h>
    #include <memory>
    #include <vector>

    #include "wrapped/Socket.hpp"
    #include "ClientData.hpp"
    #include "wrapped/Poll.hpp"

    #define LISTEN_BACKLOG 10000

namespace ftp
{
    class Server {
        public:
            Server(int port, char const *homePath);
            ~Server() = default;

            std::vector<struct pollfd> getFds() const { return _fds; }

            struct pollfd addFdToServer(int fd);
            void connectClient();
            void handleClients();
            static void setAddress(struct sockaddr_in &address, int family, u_int16_t port, in_addr_t s_addr);

        protected:
        private:
            void updateClientsPollFd();
            void handleClient(ClientData &client);
            Socket openDataSocket(int clientFd) const;

            std::shared_ptr<Socket> _serverSocket;
            struct sockaddr_in _address;
            std::vector<struct pollfd> _fds;
            std::vector<ClientData> _clients;
    };
} // namespace ftp

#endif /* !SERVER_HPP_ */
