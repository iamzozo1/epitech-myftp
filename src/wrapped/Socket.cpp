/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Socket
*/

#include "wrapped/Socket.hpp"

namespace ftp
{
    Socket::Socket(int domain, int type, int protocole) : _fd(ERROR)
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
            Close closeFd(_fd);
        }
    }
} // namespace ftp
