/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Poll
*/

#include "wrapped/Poll.hpp"

namespace ftp
{
    Poll::Poll() : _ret(ERROR)
    {}

    int Poll::pollAction(struct pollfd *fds, nfds_t nfds, int timeout)
    {
        int ret = poll(fds, nfds, timeout);

        if (ret == ERROR)
            throw Error("Poll error");
        _ret = ret;
    }

} // namespace ftp
