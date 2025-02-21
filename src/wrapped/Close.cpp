/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Close
*/

#include "wrapped/Close.hpp"

namespace ftp
{
    Close::Close(int fd)
    {
        if (close(fd) == ERROR) {
            throw Error("Unable to close fd");
        }
    }

} // namespace ftp
