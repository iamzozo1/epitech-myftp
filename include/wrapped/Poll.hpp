/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Poll
*/

#ifndef POLL_HPP_
    #define POLL_HPP_
    #include <poll.h>
    #include "Error.hpp"

namespace ftp
{
    class Server;

    class Poll {
        public:
            Poll();
            ~Poll() = default;

            int getRet() const { return _ret; }
            int pollAction(Server &s, struct pollfd *fds, nfds_t nfds, int timeout);

        private:
            int _ret;
    };

} // namespace ftp

#endif /* !POLL_HPP_ */
