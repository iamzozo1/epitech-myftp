/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Error
*/

#include "Exceptions.hpp"

using namespace ftp;

const char* Error::what() const noexcept
{
    return _message.c_str();
}

const char* FileOpenError::what() const noexcept
{
    return "450 Failed to open file";
}

const char* DataSocketWriteError::what() const noexcept
{
    return "450 Data socket write failed";
}

const char* DataSocketNullError::what() const noexcept
{
    return "503 Bad sequence of commands. Use PASV first";
}

const char* InvalidCommandError::what() const noexcept
{
    return "500 Syntax error, command unrecognized. This may include errors such as command line too long.";
}

const char* ConnectionClosed::what() const noexcept
{
    return "221 Service closing control connection.";
}
