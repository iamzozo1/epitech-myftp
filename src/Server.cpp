/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Server
*/

#include "Server.hpp"

using namespace ftp;

void Server::setServerAddress(int family, u_int16_t port, in_addr_t s_addr)
{
    _address = {0};
    _address.sin_family = family;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = s_addr;
}

Server::Server(int port, char const *homePath) : _serverSocket(nullptr)
{
    (void)homePath;
    _serverSocket = std::make_unique<Socket>(AF_INET, SOCK_STREAM, 0);
    setServerAddress(AF_INET, port, INADDR_ANY);
    Bind(_serverSocket->getFd(), (struct sockaddr *)&_address, sizeof(_address));
    Listen(_serverSocket->getFd(), LISTEN_BACKLOG);
}
