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
    #include <unordered_map>

    #include "wrapped/Socket.hpp"
    #include "ClientData.hpp"
    #include "wrapped/Poll.hpp"

    #define LISTEN_BACKLOG 10000
    #define COMMAND_SIZE 4

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
            void updateFdsAfterPoll(struct pollfd *);
            static void setAddress(struct sockaddr_in &address, int family, u_int16_t port, in_addr_t s_addr);
            CommandName getClientCommand(std::string buffer) const;

            std::string getHomePath() const { return _homePath; };

        protected:
        private:
            void updateClientsPollFd();
            void handleClient(ClientData &client);
            void closeClientConnection(unsigned int clientNb);
            Socket openDataSocket(int clientFd) const;

            std::shared_ptr<Socket> _serverSocket;
            struct sockaddr_in _address;
            std::vector<struct pollfd> _fds;
            std::vector<ClientData> _clients;
            std::string _homePath;
    };
} // namespace ftp

#endif /* !SERVER_HPP_ */
