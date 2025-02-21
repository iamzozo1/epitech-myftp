/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Error
*/

#include "Error.hpp"

using namespace ftp;

const char* Error::what() const noexcept
{
    return _message.c_str();
}