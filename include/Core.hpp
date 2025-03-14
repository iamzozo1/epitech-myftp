/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Core
*/

#ifndef CORE_HPP_
    #define CORE_HPP_

    #include "Server.hpp"

    #define WAIT_FOR_EVENT -1
    #define SUCCESS 0

namespace ftp
{
    class Core {
        public:
            Core(int, char const *);
            ~Core() = default;

            void start(void);

        private:
            std::unique_ptr<Server> _server;
    };
} // namespace ftp

#endif /* !CORE_HPP_ */
