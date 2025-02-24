/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** Core
*/

#include "Core.hpp"

namespace ftp
{
    void Core::start(Server *s)
    {
        Poll p;
        std::vector<struct pollfd> fds;

        while (true) {
            fds = s->getFds();
            p.pollAction(fds.data(), fds.size(), 0);

            if (fds[0].revents & POLLIN) {
                s->connectClient();
            } else {
                s->handleClients();
            }
        }
    }

    Core::Core(int port, char const *homePath)
    {
        try
        {
            Server s = Server(port, homePath);
            start(&s);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
} // namespace ftp
