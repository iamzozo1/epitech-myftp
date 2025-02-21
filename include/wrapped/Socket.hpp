/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Socket
*/

#ifndef SOCKET_HPP_
    #define SOCKET_HPP_

    #include <sys/socket.h>

    #include "Error.hpp"
    #include "wrapped/Close.hpp"

namespace ftp
{
    class Socket {
        public:
            Socket(int domain, int type, int protocole = 0);
            ~Socket();

            int getFd() const { return _fd; };

        protected:
        private:
            int _fd;
    };

} // namespace ftp


#endif /* !SOCKET_HPP_ */
