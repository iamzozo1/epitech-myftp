/*
** EPITECH PROJECT, 2025
** FTP
** File description:
** ClientData
*/

#include "ClientData.hpp"
#include "Server.hpp"

namespace ftp
{
    ClientData::ClientData(std::shared_ptr<struct pollfd> pollfd, std::shared_ptr<Socket> socket, std::shared_ptr<Socket> dataSocket) : _socket(socket), _dataSocket(dataSocket), _pollfd(pollfd)
    {
    }

    void ClientData::setSocket(std::shared_ptr<Socket> socket)
    {
        if (socket == nullptr)
            return;
        _socket = socket;
    }

    void ClientData::openDataSocket(void)
    {
        if (_socket == nullptr) {
            throw Error("Unable to open data socket with no control socket opened");
        }
        struct sockaddr_in data_addr;
        struct sockaddr_in bound_addr;
        socklen_t addr_len = sizeof(bound_addr);
        Socket dataSocket = Socket(AF_INET, SOCK_STREAM, 0);

        Server::setAddress(data_addr, AF_INET, INADDR_ANY, 0);
        dataSocket.setSockAddress((struct sockaddr *)&data_addr, sizeof(data_addr));
        dataSocket.bind();
        dataSocket.listen(LISTEN_BACKLOG);
        dataSocket.getSockName();

        int port = ntohs(bound_addr.sin_port);
        char response[256];
        snprintf(response, sizeof(response), "227 Entering Passive Mode (127,0,0,1,%d,%d)\r\n", port / 256, port % 256);
        _socket->write(response);

        _dataSocket = std::make_shared<Socket>(dataSocket);
    }

    void ClientData::sendFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        char buffer[1024];

        if (!file.is_open()) {
            _socket->write("FTP_ERROR: Failed to open file\r\n");
            return;
        }

        _socket->write("FTP_FILE_OK: File status okay\r\n");

        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            _dataSocket->write(buffer);
        }

        file.close();
        _socket->write("FTP_TRANSFER_COMPLETE: Transfer complete\r\n");
    }

    void ClientData::command(CommandName cmd, std::string buffer)
    {
        char filepath[1024];

        if (_dataSocket == nullptr) {
            _socket->write("FTP_ERROR: Use PASV first\r\n");
            return;
        }
        sscanf(buffer.c_str(), "RETR %s", filepath);
        _dataSocket->accept(NULL, NULL);
        sendFile(filepath);
        _dataSocket.reset();
    }

} // namespace ftp
