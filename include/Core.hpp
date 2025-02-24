/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Core
*/

#ifndef CORE_HPP_
    #define CORE_HPP_

    #include "Server.hpp"

namespace ftp
{
    class Core {
        public:
            Core(int, char const *);
            ~Core() = default;


        private:
            void start(Server *s);
    };
} // namespace ftp

#endif /* !CORE_HPP_ */
