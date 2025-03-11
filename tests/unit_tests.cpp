/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** unit_tests
*/

#include <criterion/criterion.h>
#include "Server.hpp"
// #include "ClientData.hpp"
// #include "wrapped/Socket.hpp"

Test(Server, creation)
{
    ftp::Server server(3000, ".");

    cr_assert_not_null(&server, "Server instance should not be null");
}

Test(ClientData, command_USER)
{
    try {
        std::shared_ptr<struct pollfd> pollfd = std::make_shared<struct pollfd>();
        std::shared_ptr<ftp::Socket> socket = std::make_shared<ftp::Socket>(AF_INET, SOCK_STREAM, 0);
        ftp::ClientData clientData(pollfd, socket);

        std::string buffer = "USER testuser";
        clientData.command(ftp::USER, buffer);

        std::string expectedUser = "testuser";
        std::string actualUser = clientData.getUser();

        cr_assert_eq(actualUser, expectedUser, "Expected user to be '%s', but got '%s'", expectedUser.c_str(), actualUser.c_str());
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        cr_assert_fail("Caught an exception");
    }
}
