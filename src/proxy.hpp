#pragma once

#include <cstdint>
#include <memory>
#include <print>
#include <string>
#include <string_view>
#include <thread>

#include "client.hpp"
#include "server.hpp"

namespace lz {

class proxy {
    server listener;
    std::string upstream_host;
    int upstream_port;

    template <typename T>
    static void print_array(const std::string_view name, const T& values)
    {
        std::print("world.{} = {{", name);
        bool first = true;
        for (const auto value : values) {
            if (!first)
                std::print(", ");
            std::print("{}", static_cast<unsigned long long>(value));
            first = false;
        }
        std::println("}};");
    }

    template <unsigned int bits>
    static void print_bitset(const std::string_view name, const bitset<bits>& values)
    {
        for (unsigned int bit = 0; bit < bits; ++bit)
            std::println("world.{}[{}] = {};", name, bit, values[bit]);
    }

    static void print_world_meta_mock(const netmessage::initial_world_meta& world)
    {
        std::println("netmessage::initial_world_meta world{{}};");
        std::println("world.time = {};", world.time);
        print_bitset("time_flags", world.time_flags);
        std::println("world.moon_phase = {};", world.moon_phase);
        std::println("world.max_tiles = {{{}, {}}};", world.max_tiles.x, world.max_tiles.y);
        std::println("world.spawn_tile = {{{}, {}}};", world.spawn_tile.x, world.spawn_tile.y);
        std::println("world.world_surface = {};", world.world_surface);
        std::println("world.rock_layer = {};", world.rock_layer);
        std::println("world.world_id = {};", world.world_id);
        std::println("world.world_name = \"{}\";", world.world_name);
        std::println("world.game_mode = {};", world.game_mode);
        print_array("unique_id", world.unique_id);
        std::println("world.world_generator_version = {};", world.world_generator_version);
        std::println("world.moon_type = {};", world.moon_type);
        print_array("background_styles", world.background_styles);
        std::println("world.ice_back_style = {};", world.ice_back_style);
        std::println("world.jungle_back_style = {};", world.jungle_back_style);
        std::println("world.hell_back_style = {};", world.hell_back_style);
        std::println("world.wind_speed_target = {};", world.wind_speed_target);
        std::println("world.num_clouds = {};", world.num_clouds);
        print_array("tree_x", world.tree_x);
        print_array("tree_style", world.tree_style);
        print_array("cave_back_x", world.cave_back_x);
        print_array("cave_back_style", world.cave_back_style);
        print_array("tree_top_variations", world.tree_top_variations);
        std::println("world.max_raining = {};", world.max_raining);
        print_bitset("world_flags", world.world_flags);
        print_bitset("event_flags", world.event_flags);
        print_bitset("time_event_flags", world.time_event_flags);
        print_bitset("boss_flags", world.boss_flags);
        print_bitset("invasion_flags", world.invasion_flags);
        print_bitset("progression_flags", world.progression_flags);
        print_bitset("npc_flags", world.npc_flags);
        print_bitset("secret_world_flags", world.secret_world_flags);
        print_bitset("slime_flags", world.slime_flags);
        print_bitset("seed_flags", world.seed_flags);
        print_bitset("special_world_flags", world.special_world_flags);
        std::println("world.sundial_cooldown = {};", world.sundial_cooldown);
        std::println("world.moondial_cooldown = {};", world.moondial_cooldown);
        std::println("world.saved_ore_copper = {};", world.saved_ore_copper);
        std::println("world.saved_ore_iron = {};", world.saved_ore_iron);
        std::println("world.saved_ore_silver = {};", world.saved_ore_silver);
        std::println("world.saved_ore_gold = {};", world.saved_ore_gold);
        std::println("world.saved_ore_cobalt = {};", world.saved_ore_cobalt);
        std::println("world.saved_ore_mythril = {};", world.saved_ore_mythril);
        std::println("world.saved_ore_adamantite = {};", world.saved_ore_adamantite);
        std::println("world.invasion_type = {};", world.invasion_type);
        std::println("world.lobby_id = {};", world.lobby_id);
        std::println("world.sandstorm_intended_severity = {};", world.sandstorm_intended_severity);
        std::println("world.extra_spawn_points.v = {{", "");
        for (const auto point : world.extra_spawn_points)
            std::println("    {{{}, {}}},", point.x, point.y);
        std::println("}};");
        std::println("world.dungeon = {{{}, {}}};", world.dungeon.x, world.dungeon.y);
    }

    static void forward(conn& from, conn& to, const bool inspect_world_meta)
    {
        while (true) {
            auto [id, payload] = from.read_raw();

            if (inspect_world_meta && id == netmessage::msg_id_<netmessage::initial_world_meta>()) {
                netmessage::initial_world_meta world;
                buffered_io io(payload);
                auto size = io.read(world);
                assert(size == payload.size());
                print_world_meta_mock(world);
            }

            to.write_raw(id, payload);
        }
    }

    void handle(conn client_connection) const
    {
        client upstream_connection = client(upstream_host, upstream_port);
        auto upstream = std::make_shared<conn>(upstream_connection.connect());
        auto downstream = std::make_shared<conn>(std::move(client_connection));

        std::jthread client_to_server([downstream, upstream] {
            try {
                forward(*downstream, *upstream, false);
            } catch (const std::exception& error) {
                std::println("client->server: {}", error.what());
            }
        });

        std::jthread server_to_client([upstream, downstream] {
            try {
                forward(*upstream, *downstream, true);
            } catch (const std::exception& error) {
                std::println("server->client: {}", error.what());
            }
        });
    }

public:
    proxy(const std::string_view listen_host, const int listen_port,
          std::string upstream_host, const int upstream_port)
        : listener(listen_host, listen_port)
        , upstream_host(std::move(upstream_host))
        , upstream_port(upstream_port)
    {
    }

    void run() {
        listener.bind();
        listener.listen(16);

        while (true) {
            std::thread(&proxy::handle, this, listener.accept()).detach();
        }
    }
};

}