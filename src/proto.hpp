#pragma once

#include <cstdint>
#include <meta>
#include <string>

#include "bitset.hpp"
#include "types.hpp"

namespace lz {

namespace proto {

static constexpr auto version = 326;
static constexpr auto version_string = "Terraria326";

struct header {
    uint16_t len;
    uint8_t id;
};

}

namespace netmessage {
struct message_id {
    uint8_t value;

    constexpr message_id(uint8_t value) : value(value) {}

    template <std::unsigned_integral T>
    constexpr operator T() const { return value; }

    constexpr auto operator==(const message_id&) const -> bool = default;
};

template <typename T>
consteval bool is_netmessage() {
    constexpr auto a = std::define_static_array(std::meta::annotations_of_with_type(^^T, ^^message_id));
    return a.size() == 1;
}

template <typename T>
concept netmessage = requires {
    requires is_netmessage<T>();
};

template <netmessage T>
constexpr auto id = std::meta::extract<message_id>(std::meta::annotations_of_with_type(^^T, ^^message_id).at(0));

// NOLINTBEGIN
struct [[=message_id{1}]] connect {
    std::string version;
};

struct [[=message_id{2}]] disconnect {
    nstring reason;
};

struct [[=message_id{3}]] accept {
    uint8_t slot;
    uint8_t unused = 0;
};

struct [[=message_id{4}]] player_info {
    uint8_t slot;
    uint8_t skin_variant;
    uint8_t voice_variant;
    float voice_pitch_offset;
    uint8_t hair;
    std::string name;
    uint8_t hair_dye;

    uint16_t hide_visible_accessory_mask;
    uint8_t hide_misc;

    rgb hair_color;
    rgb skin_color;
    rgb eye_color;
    rgb shirt_color;
    rgb undershirt_color;
    rgb pants_color;
    rgb shoe_color;

    bitset<4> flags;
    bitset<5> torch_flags;
    bitset<7> consumed_flags;
};

struct [[=message_id{5}]] player_inventory_slot {
    enum class inventory_slot_flags : int {
        favourite,
        blocked_slot,
    };
    uint8_t slot;

    uint16_t id;
    uint16_t stack;
    uint8_t prefix;
    uint16_t type;

    bitset<2> flags;
};

struct [[=message_id{6}]] request_initial_world_meta {};

struct x {
    std::meta::info i;
};

struct [[=message_id{7}]] initial_world_meta {
    int32_t time;

    enum class time_flags : int {
        day_time,
        blood_moon,
        eclipse,
    };
    bitset<3> time_flags;

    uint8_t moon_phase;

    vec2<int16_t> max_tiles;
    vec2<int16_t> spawn_tile;

    int16_t world_surface;
    int16_t rock_layer;

    int32_t world_id;
    std::string world_name;

    uint8_t game_mode;

    std::array<uint8_t, 16> unique_id;

    uint64_t world_generator_version;

    uint8_t moon_type;

    std::array<uint8_t, 13> background_styles;

    uint8_t ice_back_style;
    uint8_t jungle_back_style;
    uint8_t hell_back_style;

    float wind_speed_target;
    uint8_t num_clouds;

    std::array<int32_t, 3> tree_x;
    std::array<uint8_t, 4> tree_style;

    std::array<int32_t, 3> cave_back_x;
    std::array<uint8_t, 4> cave_back_style;

    std::array<uint8_t, 13> tree_top_variations;

    float max_raining;

    enum class world_flags : int {
        shadow_orb_smashed,
        downed_boss_1,
        downed_boss_2,
        downed_boss_3,
        hard_mode,
        downed_clown,
        server_side_character,
        downed_plant_boss,
    };
    bitset<8> world_flags;

    enum class event_flags : int {
        downed_mech_boss_1,
        downed_mech_boss_2,
        downed_mech_boss_3,
        downed_mech_boss_any,
        cloud_bg_active,
        crimson,
        pumpkin_moon,
        snow_moon,
    };
    bitset<8> event_flags;

    enum class time_event_flags : int {
        unused_0,
        fast_forward_time_to_dawn,
        slime_rain,
        downed_slime_king,
        downed_queen_bee,
        downed_fishron,
        downed_martians,
        downed_ancient_cultist,
    };
    bitset<8> time_event_flags;

    enum class boss_flags : int {
        downed_moonlord,
        downed_halloween_king,
        downed_halloween_tree,
        downed_christmas_ice_queen,
        downed_christmas_santank,
        downed_christmas_tree,
        downed_golem_boss,
        birthday_party_manual,
    };
    bitset<8> boss_flags;

    enum class invasion_flags : int {
        downed_pirates,
        downed_frost,
        downed_goblins,
        sandstorm,
        dd2_event_ongoing,
        dd2_invasion_t1_down,
        dd2_invasion_t2_down,
        dd2_invasion_t3_down,
    };
    bitset<8> invasion_flags;

    enum class progression_flags : int {
        combat_book_was_used,
        manual_lanterns,
        downed_tower_solar,
        downed_tower_vortex,
        downed_tower_nebula,
        downed_tower_stardust,
        force_halloween_for_today,
        force_xmas_for_today,
    };
    bitset<8> progression_flags;

    enum class npc_flags : int {
        bought_cat,
        bought_dog,
        bought_bunny,
        free_cake,
        drunk_world,
        downed_empress_of_light,
        downed_queen_slime,
        get_good_world,
    };
    bitset<8> npc_flags;

    enum class secret_world_flags : int {
        tenth_anniversary_world,
        dont_starve_world,
        downed_deerclops,
        not_the_bees_world,
        remix_world,
        unlocked_slime_blue_spawn,
        combat_book_volume_two_was_used,
        peddlers_satchel_was_used,
    };
    bitset<8> secret_world_flags;

    enum class slime_flags : int {
        unlocked_slime_green_spawn,
        unlocked_slime_old_spawn,
        unlocked_slime_purple_spawn,
        unlocked_slime_rainbow_spawn,
        unlocked_slime_red_spawn,
        unlocked_slime_yellow_spawn,
        unlocked_slime_copper_spawn,
        fast_forward_time_to_dusk,
    };
    bitset<8> slime_flags;

    enum class seed_flags : int {
        no_traps_world,
        zenith_world,
        unlocked_truffle_spawn,
        vampire_seed,
        infected_seed,
        team_based_spawns_seed,
        skyblock_world,
        dual_dungeons_seed,
    };
    bitset<8> seed_flags;

    enum class special_world_flags : int {
        skyblock_low_tiles,
        force_halloween_forever,
        force_xmas_forever,
        more_lightning_seed,
        no_lightning_seed,
    };
    bitset<5> special_world_flags;

    uint8_t sundial_cooldown;
    uint8_t moondial_cooldown;

    int16_t saved_ore_copper;
    int16_t saved_ore_iron;
    int16_t saved_ore_silver;
    int16_t saved_ore_gold;
    int16_t saved_ore_cobalt;
    int16_t saved_ore_mythril;
    int16_t saved_ore_adamantite;

    int8_t invasion_type;

    uint64_t lobby_id;

    float sandstorm_intended_severity;

    [[=encoders::prefixed_vector<uint8_t>()]]
    std::vector<vec2<int16_t>> extra_spawn_points;

    vec2<int16_t> dungeon;
};

struct [[=message_id{8}]] request_spawn {
    vec2<int32_t> position;
    uint8_t team;
};

struct [[=message_id{9}]] set_loading_message {
    int32_t status_max; // number of sections + portal sections
    nstring status_text;

    enum class status_flags : int {
        hide,
        shadows,
    };
    bitset<2> flags;
};

struct [[=message_id{10}, =io::compressed]] tile_block {
    int32_t x_start = 0;
    int32_t y_start = 0;
    int16_t width = 20;
    int16_t height = 1;

    struct mock_tile_line {
        uint8_t type = 1;
        uint8_t count = 20;

        ssize_t write(io &io) const {
            if (count == 0)
                return 0;

            ssize_t offset = 0;

            // Active tile + uint8 RLE.
            offset += io.write(uint8_t{0x42});
            offset += io.write(type);

            // Number of additional copies.
            offset += io.write(static_cast<uint8_t>(count - 1));

            return offset;
        }
    };

    mock_tile_line tiles;

    int16_t chest_count = 0;
    int16_t sign_count = 0;
    int16_t tile_entity_count = 0;

    ssize_t write(io &io) const {
        ssize_t offset = 0;

        offset += io.write(x_start);
        offset += io.write(y_start);
        offset += io.write(width);
        offset += io.write(height);

        offset += tiles.write(io);

        offset += io.write(chest_count);
        offset += io.write(sign_count);
        offset += io.write(tile_entity_count);

        return offset;
    }
};

struct [[=message_id{12}]] spawn_player {
    enum class context : uint8_t {
        revive_from_death,
        spawn_into_world,
        recall_from_item,
        team_swap,
    };

    uint8_t         slot;
    vec2<int16_t>   pos;
    int32_t         timer;
    int16_t         deaths_pve;
    int16_t         deaths_pvp;
    uint8_t         team;
    context         context;
};


struct [[=message_id{16}]] player_health {
    uint8_t slot;
    int16_t health;
    int16_t max_health;
};

struct [[=message_id{42}]] player_mana {
    uint8_t slot;
    int16_t mana;
    int16_t max_mana;
};

struct [[=message_id{49}]] player_enter {};

struct [[=message_id{50}]] player_buffs {
    uint8_t slot;

    [[=encoders::nullterm_vector{}]]
    std::vector<uint16_t> buffs;
};

struct [[=message_id{68}]] client_uuid {
    std::string uuid;
};

struct [[=message_id{129}]] finished_connecting {};

struct [[=message_id{147}]] player_loadout {
    uint8_t slot;
    uint8_t loadout_id;

    uint16_t hide_visible_accessory_mask;
};
// NOLINTEND

}
}

consteval auto known_messages() {
    using namespace std::meta;
    using namespace lz::netmessage;

    std::array<std::string_view, 256> names{};
    static constexpr auto ctx = access_context::current();

    static constexpr auto ms = std::define_static_array(
        members_of(^^lz::netmessage, ctx));

    template for (constexpr auto e : ms) {
        if constexpr (is_type(e) && is_class_type(e))
            if constexpr (netmessage<typename [: e :]>)
                names[id<typename [: e :]>] = display_string_of(e);
    }
    return names;
}

template<>
struct std::hash<lz::netmessage::message_id> {
    constexpr std::size_t operator()(lz::netmessage::message_id id) const noexcept {
        return id.value;
    }
};

template<>
struct std::formatter<lz::netmessage::message_id, char> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<class FmtContext>
    auto format(lz::netmessage::message_id s, FmtContext& ctx) const {
        static constexpr auto names = known_messages();

        const auto name = names[static_cast<uint8_t>(s)];

        if (name.empty()) {
            return std::format_to(
                ctx.out(),
                "lz::netmessage({})",
                static_cast<std::uint8_t>(s)
            );
        }

        return std::format_to(ctx.out(), "{}", name);
    }
};