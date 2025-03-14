/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Core
*/

#include "Core.hpp"

namespace ftp
{
    void Core::start(void)
    {
        Poll p;
        std::vector<struct pollfd> fds;

        while (true) {
            fds = _server->getFds();
            p.pollAction(*_server, fds.data(), fds.size(), WAIT_FOR_EVENT);

            if (fds[0].revents & POLLIN) {
                _server->connectClient();
            } else {
                _server->handleClients();
            }
        }
    }

    Core::Core(int port, char const *homePath)
    {
        _server = std::make_unique<Server>(port, homePath);
    }
} // namespace ftp
