/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** main
*/

#include "Core.hpp"

using namespace ftp;

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "USAGE: " << av[0] << " port path" << std::endl;
        return 84;
    }

    Core c = Core(std::stoi(av[1]), av[2]);
    return SUCCESS;
}