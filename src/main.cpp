/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** main
*/

#include "Server.hpp"

using namespace ftp;

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "USAGE: " << av[0] << " port path" << std::endl;
        return 84;
    }

    int port = std::stoi(av[1]);
    const char *homePath = av[2];

    Server server = Server(port, homePath);

    return 0;
}