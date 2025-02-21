/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** unit_tests
*/

#include <criterion/criterion.h>
#include "Server.hpp"

Test(Server, creation)
{
    printf("before test creat\n");
    ftp::Server server;

    cr_assert_not_null(&server, "Server instance should not be null");
}

Test(Server, socket_initialization)
{
    printf("before test init\n");
    ftp::Server server;

    int fd = server.getSocketFd();
    cr_assert_neq(fd, -1, "Socket file descriptor should not be -1");
}

Test(Server, address_initialization)
{
    printf("before test adder\n");
    ftp::Server server;

    struct sockaddr_in address = server.getAddress();
    cr_assert_eq(address.sin_family, AF_INET, "Address family should be AF_INET");
    cr_assert_eq(address.sin_port, htons(PORT), "Port should be set to defined PORT");
    cr_assert_eq(address.sin_addr.s_addr, INADDR_ANY, "Address should be set to INADDR_ANY");
}