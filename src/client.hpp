#pragma once

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string_view>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "conn.hpp"

namespace lz {

class client {
    sockaddr_in server_address {};

public:
    client(const std::string_view hostname, const int port)
    {
        server_address.sin_family = AF_INET;
        inet_pton(AF_INET, hostname.data(), &server_address.sin_addr);
        server_address.sin_port = htons(port);
    }

    conn connect()
    {
        const int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0)
            throw std::runtime_error(strerror(errno));

        const auto err = ::connect(sock_fd, reinterpret_cast<struct sockaddr *>(&server_address),
            sizeof(server_address));
        if (err < 0)
            throw std::runtime_error(strerror(errno));

        return conn { sock_fd, server_address, sizeof(server_address) };
    }
};
};