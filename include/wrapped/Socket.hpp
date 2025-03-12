/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Socket
*/

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <sys/socket.h>
#include <unistd.h>

#include "Exceptions.hpp"
#define CRLF "\r\n"

namespace ftp
{
    class Socket
    {
    public:
        Socket(int domain, int type, int protocole = 0);
        Socket(int fd);
        ~Socket();

        void setSockAddress(struct sockaddr *addr, socklen_t addrlen);

        void bind();
        void getSockName();
        int accept(struct sockaddr *addr, socklen_t *addrlen);
        void listen(int backlog);
        ssize_t write(std::string buf);
        ssize_t read(char *buf, size_t count);
        void connect();

        int _fd;

    private:
        struct sockaddr *_addr;
        socklen_t _addrlen;
    };

} // namespace ftp

#endif /* !SOCKET_HPP_ */
