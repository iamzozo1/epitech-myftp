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

    class FileOpenError : public std::exception {
        public:
            const char* what() const noexcept override;
    };

    class DataSocketWriteError : public std::exception {
        public:
            const char* what() const noexcept override;
    };

    class DataSocketNullError : public std::exception {
        public:
            const char* what() const noexcept override;
    };

    class InvalidCommandError : public std::exception {
        public:
            const char* what() const noexcept override;
    };

    class ConnectionClosed : public std::exception {
        public:
            const char* what() const noexcept override;
    };

    class ReadError : public std::exception {
        public:
            ReadError() {
                _message = "Read failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

    class WriteError : public std::exception {
        public:
            WriteError() {
                _message = "Write failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

    class AcceptError : public std::exception {
        public:
            AcceptError() {
                _message = "Accept failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

    class RemoveError : public std::exception {
        public:
            RemoveError() {
                _message = "Remove failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

    class ConnectError : public std::exception {
        public:
            ConnectError() {
                _message = "Connect failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

    class ChdirError : public std::exception {
        public:
            ChdirError() {
                _message = "Chdir failed: ";
                _message += + std::strerror(errno);
            };
            const char* what() const noexcept override;
        private:
            std::string _message;
    };

} // namespace ftp


#endif /* !ERROR_HPP_ */
