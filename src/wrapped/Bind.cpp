/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Bind
*/

#include "wrapped/Bind.hpp"

namespace ftp
{
    Bind::Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        if (bind(sockfd, addr, addrlen) == ERROR) {
            Close closeSocket(sockfd);
            throw Error("Unable to bind socket fd");
        }
    }

} // namespace ftp

