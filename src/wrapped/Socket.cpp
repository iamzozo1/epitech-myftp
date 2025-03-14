/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Socket
*/

#include "wrapped/Socket.hpp"

namespace ftp
{
    Socket::Socket(int domain, int type, int protocole) : _fd(ERROR), _addr(NULL)
    {
        int fd = socket(domain, type, 0);

        if (fd == ERROR) {
            throw Error("Socket initialization");
        }
        _fd = fd;
    }

    Socket::Socket(int fd) : _fd(fd), _addr(NULL)
    {}

    Socket::~Socket()
    {
        if (_fd != ERROR) {
            close(_fd);
        }
    }

    void Socket::setSockAddress(struct sockaddr *addr, socklen_t addrlen)
    {
        _addr = addr;
        _addrlen = addrlen;
    }

    void Socket::getSockName()
    {
        if (_fd == ERROR)
            throw Error("Unable to getsockname of closed fd");
        if (getsockname(_fd, _addr, &_addrlen) == ERROR) {
            throw Error("getsockname");
        }
    }

    int Socket::accept(struct sockaddr *addr, socklen_t *addrlen)
    {
        if (_fd == ERROR)
            throw Error("Unable to accept with closed fd");
        int ret = ::accept(_fd, addr, addrlen);

        if (ret == ERROR)
            throw AcceptError();
        return ret;
    }

    void Socket::listen(int backlog)
    {
        if (_fd == ERROR)
            throw Error("Unable to listen to closed fd");
        if (::listen(_fd, backlog) == ERROR) {
            throw Error("Unable to listen to sockfd");
        }
    }

    void Socket::bind()
    {
        if (_fd == ERROR)
            throw Error("Unable to bind closed fd");
        if (::bind(_fd, _addr, _addrlen) == ERROR)
            throw Error("Unable to bind socket address");
    }

    ssize_t Socket::write(std::string buf)
    {
        size_t size = 0;
        size_t bytesWritten = 0;

        if (_fd == ERROR || buf.empty())
            return 0;
        buf += CRLF;
        size = buf.size();
        bytesWritten = ::write(_fd, buf.c_str(), size);
        if (bytesWritten == ERROR)
            throw WriteError();
        return bytesWritten;
    }

    ssize_t Socket::read(char *buf, size_t count)
    {
        size_t bytesRead = 0;

        if (buf == nullptr || _fd == ERROR)
            return 0;
        bytesRead = ::read(_fd, buf, count);
        if (bytesRead == ERROR)
            throw ReadError();
        return bytesRead;
    }

    void Socket::connect()
    {
        if (::connect(_fd, _addr, _addrlen) == ERROR)
            throw ConnectError();
    }
} // namespace ftp
