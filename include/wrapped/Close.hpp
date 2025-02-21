/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Close
*/

#ifndef CLOSE_HPP_
    #define CLOSE_HPP_

    #include <unistd.h>
    
    #include "Error.hpp"

namespace ftp
{
    class Close {
        public:
            Close(int fd);
            ~Close() = default;

    };

} // namespace ftp


#endif /* !CLOSE_HPP_ */
