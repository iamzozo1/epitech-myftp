/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Error
*/

#ifndef ERROR_HPP_
    #define ERROR_HPP_
    #include <string>
    #include <cstring>
    #include "string.h"

    #define ERROR -1

namespace ftp
{
    class Error : public std::exception {
        public:
            Error(std::string message = "") {
                if (!message.empty())
                    _message = message + ": " + std::strerror(errno);
                else
                    _message = std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

} // namespace ftp


#endif /* !ERROR_HPP_ */
