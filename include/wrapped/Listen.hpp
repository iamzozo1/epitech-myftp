/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Listen
*/

#ifndef LISTEN_HPP_
    #define LISTEN_HPP_

    #include <sys/socket.h>
    #include <unistd.h>
    #include "Error.hpp"
    #include "wrapped/Close.hpp"

namespace ftp
{
    class Listen {
        public:
            Listen(int sockfd, int backlog);
            ~Listen() = default;
    };

} // namespace ftp

#endif /* !LISTEN_HPP_ */
