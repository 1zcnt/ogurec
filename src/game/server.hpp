#pragma once

#include <atomic>
#include <format>
#include <mutex>
#include <thread>
#include <utility>

#include "player.hpp"
#include "../server.hpp"

namespace lz::game {
static netmessage::initial_world_meta mock_info = {
    .time = 27000,
    .time_flags = {1},
    .moon_phase = 0,
    .max_tiles = {.x = 4200, .y = 1200},
    .spawn_tile = {.x = 100, .y = 100},
    .world_surface = 105,
    .rock_layer = 400,
    .world_id = 2029199824,
    .world_name = "ogurec",
    .game_mode = 0,
    .unique_id = {222, 9, 5, 113, 63, 72, 23, 73, 131, 181, 158, 240, 84, 216, 57, 196},
    .world_generator_version = 1400159338497,
    .moon_type = 0,
    .background_styles = {12, 8, 11, 9, 2, 0, 6, 2, 3, 4, 4, 3, 1},
    .ice_back_style = 0,
    .jungle_back_style = 1,
    .hell_back_style = 1,
    .wind_speed_target = 0,
    .num_clouds = 0,
    .tree_x = {2067, 4200, 4200},
    .tree_style = {5, 4, 0, 0},
    .cave_back_x = {2012, 4200, 4200},
    .cave_back_style = {0, 5, 1, 2},
    .tree_top_variations = {5, 4, 0, 0, 2, 0, 6, 2, 3, 4, 4, 3, 1},
    .max_raining = 0,
    // ...(flags/bitsets/ore fields unchanged)
    .sundial_cooldown = 0,
    .moondial_cooldown = 0,
    .saved_ore_copper = -1,
    .saved_ore_iron = -1,
    .saved_ore_silver = -1,
    .saved_ore_gold = -1,
    .saved_ore_cobalt = -1,
    .saved_ore_mythril = -1,
    .saved_ore_adamantite = -1,
    .invasion_type = 0,
    .lobby_id = 0,
    .sandstorm_intended_severity = 0,
    .extra_spawn_points = {},
    .dungeon = {},
};

class server : public lz::server {
    std::atomic<uint64_t> player_slot_mask[4];

    std::string password;

public:
    server(const std::string_view hostname, const int port, std::string_view password = "")
        : lz::server(hostname, port)
        , password(password) {}

    void run() {
        bind();
        listen(256);
        // FIXME: threads
        conn c(*this,accept());
        try {
            while (true) c.handle();
        } catch (const kick_exception &e) {
            e.write(c);
        }
    }

private:
    struct kick_exception : std::exception, netmessage::disconnect {
        [[nodiscard]] const char* what() const noexcept override {
            return reason.text.c_str();
        }
        ssize_t write(conn &c) const {
            return c.write(disconnect{*this});
        }
        template <typename ...Ts>
        kick_exception(Ts ...args) : disconnect(args...) {}
    };

    class conn : public lz::conn {
        server &srv;

        std::string nickname;
        uint8_t slot;

        enum class state : int {
            init,
            connected,
            receiving_initial_tiles,
            playing,
        };

        state current_state = state::init;

        void register_state_guards() {
            using namespace std::meta;
            template for (constexpr auto e : std::define_static_array(enumerators_of(^^enum state))) {
                constexpr auto state_str = identifier_of(e);

                register_middleware(state_str, [this] {
                    return current_state == [:e:];
                });
            }
        }

    public:
        conn(server &s, lz::conn &&c)
        : lz::conn(std::move(c))
        , srv(s) {
            register_handlers(this);
            register_state_guards();
        }

        [[=handler{.ns = "::init"}]]
        void connect_request(netmessage::connect c) {
            if (c.version != proto::version_string) {
                throw kick_exception{std::format("Incompatible version: {}. Expected: {}", c.version, proto::version_string)};
            }

            current_state = state::connected;
            if (const auto slot = srv.allocate_player_slot(); slot) {
                this->slot = *slot;
                write(netmessage::accept{this->slot});
                return;
            }
            throw kick_exception{"server is full"};
        }

        [[=handler{.ns = "::connected"}]]
        void player_initial_info(
            netmessage::player_info pi,
            netmessage::client_uuid uuid,
            netmessage::player_health health,
            netmessage::player_mana mana,
            netmessage::player_buffs buffs,
            netmessage::player_loadout loadout) {

            nickname = pi.name;
            std::println("{} connected", nickname);
        }

        [[=handler{.ns = "::connected"}]]
        void player_initial_inventory(netmessage::player_inventory_slot inv) {}

        [[=handler{.ns = "::connected"}]]
        void request_initial_world_meta(netmessage::request_initial_world_meta r) {
            write(mock_info);
            current_state = state::receiving_initial_tiles;
        }

        [[=handler{.ns = "::recieved_initial_tiles"}]]
        void request_spawn(netmessage::request_spawn r) {
            write(netmessage::set_loading_message{
                .status_max = 1,
                .status_text = "[keeek]",
            });

            write(netmessage::tile_block{
                .x_start = mock_info.spawn_tile.x - 10,
                .y_start = mock_info.spawn_tile.y + 3,
            });

            write(netmessage::player_enter{});
        }

        [[=handler{.ns = "::recieved_initial_tiles"}]]
        void spawn_player(netmessage::spawn_player s) {
            std::println("pos {} {}", s.pos.x, s.pos.y);
            write(netmessage::finished_connecting{});
            current_state = state::playing;
        }
    };

    std::optional<uint8_t> allocate_player_slot() {
        for (uint8_t idx = 0; idx < 4; ++idx) {
            auto mask = player_slot_mask[idx].load(std::memory_order_relaxed);

            while (mask != UINT64_MAX) {
                const auto bit = std::countr_zero(~mask);

                if (const auto new_mask = mask | (uint64_t{1} << bit); player_slot_mask[idx].compare_exchange_weak(
                    mask,
                    new_mask,
                    std::memory_order_acquire,
                    std::memory_order_relaxed))
                    return idx * 64 + bit;
            }
        }
        return std::nullopt;
    }

    void free_player_slot(const uint8_t slot) {
        const auto idx = slot / 64;
        const auto bit = slot % 64;

        player_slot_mask[idx].fetch_and(
            ~(uint64_t{1} << bit),
            std::memory_order_release
        );
    }

};
}
