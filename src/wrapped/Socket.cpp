/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Socket
*/

#include "wrapped/Socket.hpp"

namespace ftp
{
    Socket::Socket(int domain, int type, int protocole) : _fd(ERROR), _addr(nullptr)
    {
        int fd = socket(domain, type, 0);

        if (fd == ERROR) {
            throw Error("Socket initialization:");
        }
        _fd = fd;
    }

    Socket::~Socket()
    {
        if (_fd != ERROR) {
            if (close(_fd) == ERROR) {
                throw Error("Unable to close fd");
            }
        }
    }

    void Socket::setSockAddress(struct sockaddr *addr, socklen_t addrlen)
    {
        _addr = addr;
        _addrlen = _addrlen;
    }

    void Socket::getSockName()
    {
        if (getsockname(_fd, _addr, &_addrlen)) {
            throw Error("getsockname");
        }
    }

    int Socket::accept()
    {
        int ret = ::accept(_fd, _addr, &_addrlen);

        if (ret == ERROR)
            throw Error("Accept failed");
        return ret;
    }

    void Socket::listen(int backlog)
    {
        if (::listen(_fd, backlog) != ERROR) {
            throw Error("Unable to listen to sockfd");
        }
    }

    void Socket::bind()
    {
        if (::bind(_fd, _addr, _addrlen) == ERROR)
            throw Error("Unable to bind socket address");
    }
} // namespace ftp
