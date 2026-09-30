#pragma once

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string_view>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conn.hpp"

namespace lz {

class server {
    sockaddr_in server_address {};
    int sock_fd{};

public:
    server(const std::string_view hostname, const int port)
    {
        server_address.sin_family = AF_INET;
        inet_pton(AF_INET, hostname.data(), &server_address.sin_addr);
        server_address.sin_port = htons(port);
    }

    ~server()
    {
        close(sock_fd);
    }

    void bind()
    {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0)
            throw std::runtime_error(strerror(errno));

        constexpr int yes = 1;
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

        const int bindStatus = ::bind(sock_fd, reinterpret_cast<struct sockaddr *>(&server_address),
                                      sizeof(server_address));
        if (bindStatus < 0)
            throw std::runtime_error(strerror(errno));
    }

    void listen(const int max_connections) const
    {
        ::listen(sock_fd, max_connections);
    }

    [[nodiscard]] conn accept() const
    {
        sockaddr_in conn_addr {};
        socklen_t conn_addr_size = sizeof(conn_addr);

        const int conn_fd = ::accept(sock_fd, reinterpret_cast<struct sockaddr *>(&conn_addr), &conn_addr_size);
        if (conn_fd < 0)
            throw std::runtime_error(strerror(errno));
        return conn { conn_fd, conn_addr, conn_addr_size };
    }
};

}