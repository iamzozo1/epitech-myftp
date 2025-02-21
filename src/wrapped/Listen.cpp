/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Listen
*/

#include "wrapped/Listen.hpp"

namespace ftp
{
    Listen::Listen(int sockfd, int backlog)
    {
        if (listen(sockfd, backlog)) {
            Close closeSocket(sockfd);
            throw Error("Unable to listen to sockfd");
        }
    }

} // namespace ftp
