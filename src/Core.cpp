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
            printf("before getting fd\n");
            fds = s->getFds();
            printf("before poll action\n");
            p.pollAction(fds.data(), fds.size(), WAIT_FOR_EVENT);

            printf("checking new connection request\n");
            if (fds[0].revents & POLLIN) {
                printf("manage connection\n");
                s->connectClient();
            } else {
                printf("handle clients\n");
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
