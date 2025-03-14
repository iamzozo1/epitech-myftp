/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** main
*/

#include <filesystem>
#include "Core.hpp"

using namespace ftp;

int main(int ac, char **av)
{
    int port = 0;
    const int ERR_CODE = 84;

    if (ac != 3) {
        std::cerr << "USAGE: " << av[0] << " port path" << std::endl;
        return ERR_CODE;
    }
    try {
        port = std::stoi(av[1]);
        if (port <= 0) {
            std::cerr << "Error: The port must be a valid number between 1 and 65535." << std::endl;
            return ERR_CODE;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: The port must be a valid number." << std::endl;
        return ERR_CODE;
    }

    if (!std::filesystem::exists(av[2])) {
        std::cerr << "Error: The path does not exist." << std::endl;
        return ERR_CODE;
    }

    try {
        Core c = Core(port, av[2]);
        c.start();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return ERR_CODE;
    }
    return SUCCESS;
}