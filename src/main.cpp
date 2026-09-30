#include <charconv>
#include <exception>
#include <print>
#include <string_view>

#include "game/server.hpp"

int main(int argc, char **argv) {
    if (argc > 4) {
        std::println(stderr, "usage: {} [host] [port] [password]", argv[0]);
        return 1;
    }

    const std::string_view host     = argc > 1 ? argv[1] : "0.0.0.0";
    const std::string_view port_str = argc > 2 ? argv[2] : "7777";
    const std::string_view password = argc > 3 ? argv[3] : "";

    int port{};
    const auto [end, ec] = std::from_chars(port_str.begin(), port_str.end(), port);
    if (ec != std::errc{} || end != port_str.end() || port <= 0 || port > 65535) {
        std::println(stderr, "invalid port: {}", port_str);
        return 1;
    }

    try {
        lz::game::server srv(host, port, password);
        std::println("listening on {}:{}", host, port);
        srv.run();
    } catch (const std::exception &e) {
        std::println(stderr, "error: {}", e.what());
        return 1;
    }
}
