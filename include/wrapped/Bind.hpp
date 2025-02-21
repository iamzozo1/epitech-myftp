/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Bind
*/

#ifndef BIND_HPP_
    #define BIND_HPP_

    #include <sys/socket.h>
    #include "Error.hpp"
    #include "wrapped/Close.hpp"

namespace ftp
{
    class Bind {
        public:
            Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
            ~Bind() = default;
    };
} // namespace ftp


#endif /* !BIND_HPP_ */
