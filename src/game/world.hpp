#pragma once

#include <cstdint>
#include <print>
#include <vector>
#include <span>
#include <unordered_map>

#include "../io.hpp"
#include "../proto.hpp"
#include "../types.hpp"
#include "file.hpp"

namespace lz::game {
// static const std::unordered_map<uint16_t, std::monostate> important_tiles{
//     [3]  =  {}, [4]  =  {}, [5]  =  {}, [10] =  {}, [11] =  {},
//     [12] =  {}, [13] =  {}, [14] =  {}, [15] =  {}, [16] =  {},
//     [17] =  {}, [18] =  {}, [19] =  {}, [20] =  {}, [21] =  {},
//     [24] =  {}, [26] =  {}, [27] =  {}, [28] =  {}, [29] =  {},
//     [31] =  {}, [33] =  {}, [34] =  {}, [35] =  {}, [36] =  {},
//     [42] =  {}, [49] =  {}, [50] =  {}, [55] =  {}, [61] =  {},
//     [71] =  {}, [72] =  {}, [73] =  {}, [74] =  {}, [77] =  {},
//     [78] =  {}, [79] =  {}, [81] =  {}, [82] =  {}, [83] =  {},
//     [84] =  {}, [85] =  {}, [86] =  {}, [87] =  {}, [88] =  {},
//     [89] =  {}, [90] =  {}, [91] =  {}, [92] =  {}, [93] =  {},
//     [94] =  {}, [95] =  {}, [96] =  {}, [97] =  {}, [98] =  {},
//     [99] =  {}, [100] = {}, [101] = {}, [102] = {}, [103] = {},
//     [104] = {}, [105] = {}, [106] = {}, [110] = {}, [113] = {},
//     [114] = {}, [125] = {}, [126] = {}, [128] = {}, [129] = {},
//     [132] = {}, [133] = {}, [134] = {}, [135] = {}, [136] = {},
//     [137] = {}, [138] = {}, [139] = {}, [141] = {}, [142] = {},
//     [143] = {}, [144] = {}, [149] = {}, [165] = {}, [171] = {},
//     [172] = {}, [173] = {}, [174] = {}, [178] = {}, [184] = {},
//     [185] = {}, [186] = {}, [187] = {}, [201] = {}, [207] = {},
//     [209] = {}, [210] = {}, [212] = {}, [215] = {}, [216] = {},
//     [217] = {}, [218] = {}, [219] = {}, [220] = {}, [227] = {},
//     [228] = {}, [231] = {}, [233] = {}, [235] = {}, [236] = {},
//     [237] = {}, [238] = {}, [239] = {}, [240] = {}, [241] = {},
//     [242] = {}, [243] = {}, [244] = {}, [245] = {}, [246] = {},
//     [247] = {}, [254] = {}, [269] = {}, [270] = {}, [271] = {},
//     [275] = {}, [276] = {}, [277] = {}, [278] = {}, [279] = {},
//     [280] = {}, [281] = {}, [282] = {}, [283] = {}, [285] = {},
//     [286] = {}, [287] = {}, [288] = {}, [289] = {}, [290] = {},
//     [291] = {}, [292] = {}, [293] = {}, [294] = {}, [295] = {},
//     [296] = {}, [297] = {}, [298] = {}, [299] = {}, [300] = {},
//     [301] = {}, [302] = {}, [303] = {}, [304] = {}, [305] = {},
//     [306] = {}, [307] = {}, [308] = {}, [309] = {}, [310] = {},
//     [314] = {}, [316] = {}, [317] = {}, [318] = {}, [319] = {},
//     [320] = {}, [323] = {}, [324] = {}, [334] = {}, [335] = {},
//     [337] = {}, [338] = {}, [339] = {}, [349] = {}, [354] = {},
//     [355] = {}, [356] = {}, [358] = {}, [359] = {}, [360] = {},
//     [361] = {}, [362] = {}, [363] = {}, [364] = {}, [372] = {},
//     [373] = {}, [374] = {}, [375] = {}, [376] = {}, [377] = {},
//     [378] = {}, [380] = {}, [386] = {}, [387] = {}, [388] = {},
//     [389] = {}, [390] = {}, [391] = {}, [392] = {}, [393] = {},
//     [394] = {}, [395] = {}, [405] = {}, [406] = {}, [410] = {},
//     [411] = {}, [412] = {}, [413] = {}, [414] = {}, [419] = {},
//     [420] = {}, [423] = {}, [424] = {}, [425] = {}, [427] = {},
//     [428] = {}, [429] = {}, [440] = {}, [441] = {}, [442] = {},
//     [443] = {}, [444] = {}, [445] = {}, [452] = {}, [453] = {},
//     [454] = {}, [455] = {}, [456] = {}, [457] = {}, [461] = {},
//     [462] = {}, [463] = {}, [464] = {}, [465] = {}, [466] = {},
//     [467] = {}, [468] = {}, [469] = {}, [470] = {}, [471] = {},
//     [475] = {}, [476] = {}, [480] = {}, [484] = {}, [485] = {},
//     [486] = {}, [487] = {}, [488] = {}, [489] = {}, [490] = {},
//     [491] = {}, [493] = {}, [494] = {}, [497] = {}, [499] = {},
//     [505] = {}, [506] = {}, [509] = {}, [510] = {}, [511] = {},
//     [518] = {}, [519] = {}, [520] = {}, [521] = {}, [522] = {},
//     [523] = {}, [524] = {}, [525] = {}, [526] = {}, [527] = {},
//     [529] = {}, [530] = {}, [531] = {}, [532] = {}, [533] = {},
//     [538] = {}, [542] = {}, [543] = {}, [544] = {}, [545] = {},
//     [547] = {}, [548] = {}, [549] = {}, [550] = {}, [551] = {},
//     [552] = {}, [553] = {}, [554] = {}, [555] = {}, [556] = {},
//     [558] = {}, [559] = {}, [560] = {}, [564] = {}, [565] = {},
//     [567] = {}, [568] = {}, [569] = {}, [570] = {}, [571] = {},
//     [572] = {}, [573] = {}, [579] = {}, [580] = {}, [581] = {},
//     [582] = {}, [583] = {}, [584] = {}, [585] = {}, [586] = {},
//     [587] = {}, [588] = {}, [589] = {}, [590] = {}, [591] = {},
//     [592] = {}, [593] = {}, [594] = {}, [595] = {}, [596] = {},
//     [597] = {}, [598] = {}, [599] = {}, [600] = {}, [601] = {},
//     [602] = {}, [603] = {}, [604] = {}, [605] = {}, [606] = {},
//     [607] = {}, [608] = {}, [609] = {}, [610] = {}, [611] = {},
//     [612] = {}, [613] = {}, [614] = {}, [615] = {}, [616] = {},
//     [617] = {}, [619] = {}, [620] = {}, [621] = {}, [622] = {},
//     [623] = {}, [624] = {}, [629] = {}, [630] = {}, [631] = {},
//     [632] = {}, [634] = {}, [637] = {}, [639] = {}, [640] = {},
//     [642] = {}, [643] = {}, [644] = {}, [645] = {}, [646] = {},
//     [653] = {}, [654] = {}, [656] = {}, [657] = {}, [658] = {},
//     [660] = {}, [663] = {}, [664] = {}, [665] = {}, [695] = {},
//     [696] = {}, [698] = {}, [699] = {}, [700] = {}, [701] = {},
//     [702] = {}, [703] = {}, [704] = {}, [705] = {}, [707] = {},
//     [709] = {}, [710] = {}, [711] = {}, [712] = {}, [713] = {},
//     [714] = {}, [715] = {}, [716] = {}, [720] = {}, [721] = {},
//     [723] = {}, [724] = {}, [725] = {}, [726] = {}, [733] = {},
//     [751] = {}, [752] = {}, [753] = {},
// };

struct tile {
    enum class tile_type : uint8_t {
        solid,
        half_brick,
        slope_down_right,
        slope_down_left,
        slope_up_right,
        slope_up_left,
    };

    enum class liquid_type : uint8_t {
        water,
        lava,
        honey,
        shimmer,
    };

    uint16_t id;
    uint16_t wall;
    bool liquid;

    vec2<uint16_t> frame_pos;
};

class tile_block {
    std::vector<uint8_t> data;

    enum class tile_flags_1 {
        read_next_flags,
        active,
        read_wall_color,
        liquid_water_shimmer,
        liquid_lava_honey,
        wide_tile_id,
        rle_1,
        rle_2,
    };

    enum class tile_flags_2 {
        read_next_flags,
        read_tile_id,
    };

public:
    ssize_t write(io &io) const {
        return io.write(data);
    }

    ssize_t read(io &io) {
        data.clear();

        ssize_t offset{};
        try {
            while (true) {
                constexpr auto blk_size = 512;

                data.resize(data.size() + 512);

                auto ss = std::span(data.end() - blk_size, data.end());
                const auto bytes = io.read(ss);
                data.resize(data.size() - (blk_size - bytes));

                offset += bytes;
            }
        } catch (const io::eof &e) {
        }
        return offset;
    }
};

using guid = std::array<uint8_t, 16>;
struct world {
    // File header
    int32_t version = proto::version;
    std::string name;

    std::string seed_text;
    uint64_t world_generator_version = 0;

    std::array<uint8_t, 16> unique_id{};
    int32_t id = 0;

    // World geometry
    rect<int32_t> world_bounds{};
    vec2<int32_t> max_tiles{};
    vec2<int32_t> spawn_tile{};

    int16_t world_surface = 0;
    int16_t rock_layer = 0;
    vec2<int16_t> dungeon{};

    // World state
    int32_t time = 0;

    enum class time_flag : int {
        day_time,
        blood_moon,
        eclipse,

        flags_total,
    };
    bitset<time_flag::flags_total> time_flags;

    uint8_t moon_phase = 0;
    uint8_t moon_type = 0;

    uint8_t game_mode = 0;

    // Background / scenery
    std::array<uint8_t, 13> background_styles{};

    uint8_t ice_back_style = 0;
    uint8_t jungle_back_style = 0;
    uint8_t hell_back_style = 0;

    float wind_speed_target = 0;
    uint8_t num_clouds = 0;

    std::array<int32_t, 3> tree_x{};
    std::array<uint8_t, 4> tree_style{};

    std::array<int32_t, 3> cave_back_x{};
    std::array<uint8_t, 4> cave_back_style{};

    std::array<uint8_t, 13> tree_top_variations{};

    // Weather
    bool raining = false;
    int32_t rain_time = 0;
    float max_raining = 0;

    float sandstorm_intended_severity = 0;

    // World flags
    enum class world_flag : int {
        shadow_orb_smashed,
        downed_boss_1,
        downed_boss_2,
        downed_boss_3,
        hard_mode,
        downed_clown,
        server_side_character,
        downed_plant_boss,

        downed_mech_boss_1,
        downed_mech_boss_2,
        downed_mech_boss_3,
        downed_mech_boss_any,
        cloud_bg_active,
        crimson,
        pumpkin_moon,
        snow_moon,

        unused_0,
        fast_forward_time_to_dawn,
        slime_rain,
        downed_slime_king,
        downed_queen_bee,
        downed_fishron,
        downed_martians,
        downed_ancient_cultist,

        downed_moonlord,
        downed_halloween_king,
        downed_halloween_tree,
        downed_christmas_ice_queen,
        downed_christmas_santank,
        downed_christmas_tree,
        downed_golem_boss,
        birthday_party_manual,

        downed_pirates,
        downed_frost,
        downed_goblins,
        sandstorm,
        dd2_event_ongoing,
        dd2_invasion_t1_down,
        dd2_invasion_t2_down,
        dd2_invasion_t3_down,

        combat_book_was_used,
        manual_lanterns,
        downed_tower_solar,
        downed_tower_vortex,
        downed_tower_nebula,
        downed_tower_stardust,
        force_halloween_for_today,
        force_xmas_for_today,

        bought_cat,
        bought_dog,
        bought_bunny,
        free_cake,
        drunk_world,
        downed_empress_of_light,
        downed_queen_slime,
        get_good_world,

        tenth_anniversary_world,
        dont_starve_world,
        downed_deerclops,
        not_the_bees_world,
        remix_world,
        unlocked_slime_blue_spawn,
        combat_book_volume_two_was_used,
        peddlers_satchel_was_used,

        unlocked_slime_green_spawn,
        unlocked_slime_old_spawn,
        unlocked_slime_purple_spawn,
        unlocked_slime_rainbow_spawn,
        unlocked_slime_red_spawn,
        unlocked_slime_yellow_spawn,
        unlocked_slime_copper_spawn,
        fast_forward_time_to_dusk,

        no_traps_world,
        zenith_world,
        unlocked_truffle_spawn,
        vampire_seed,
        infected_seed,
        team_based_spawns_seed,
        skyblock_world,
        dual_dungeons_seed,

        skyblock_low_tiles,
        force_halloween_forever,
        force_xmas_forever,
        more_lightning_seed,
        no_lightning_seed,

        flags_total,
    };
    bitset<world_flag::flags_total> world_flags;

    // Progression / events not represented by initial_world_meta bitsets
    bool spawn_meteor = false;
    uint8_t shadow_orb_count = 0;
    int32_t altar_count = 0;

    bool after_party_of_doom = false;

    int32_t invasion_delay = 0;
    int32_t invasion_size = 0;
    int8_t invasion_type = 0;
    double invasion_x = 0;

    double slime_rain_time = 0;

    // Angler
    bool saved_angler = false;
    int32_t angler_quest = 0;

    // NPC progression
    bool saved_goblin = false;
    bool saved_wizard = false;
    bool saved_mech = false;
    bool saved_stylist = false;
    bool saved_tax_collector = false;
    bool saved_golfer = false;

    // Sundial / moondial
    uint8_t sundial_cooldown = 0;
    uint8_t moondial_cooldown = 0;

    // Ore tiers
    int16_t saved_ore_copper = -1;
    int16_t saved_ore_iron = -1;
    int16_t saved_ore_silver = -1;
    int16_t saved_ore_gold = -1;
    int16_t saved_ore_cobalt = -1;
    int16_t saved_ore_mythril = -1;
    int16_t saved_ore_adamantite = -1;

    // Misc progression
    bool cultist_delay = false;

    bool tower_active_solar = false;
    bool tower_active_vortex = false;
    bool tower_active_nebula = false;
    bool tower_active_stardust = false;

    bool lunar_apocalypse_is_up = false;

    // Party
    bool party_manual = false;
    bool party_genuine = false;
    int32_t party_cooldown = 0;
    std::vector<int32_t> party_npcs;

    // DD2
    // These are represented by world_flag entries.

    // Lantern
    int32_t lantern_cooldown = 0;
    bool lantern_genuine = false;
    bool lantern_manual = false;
    bool next_night_is_genuine = false;

    // Special NPC/world progression
    bool bought_merchant = false;
    bool bought_demolitionist = false;
    bool bought_party_girl = false;
    bool bought_dye_trader = false;
    bool bought_truffle = false;
    bool bought_arms_dealer = false;
    bool bought_nurse = false;
    bool bought_princess = false;

    // Extra spawn points
    [[=encoders::prefixed_vector<uint8_t>()]]
    std::vector<vec2<int16_t>> extra_spawn_points;

    // File metadata which has no direct network representation
    int64_t creation_time = 0;
    int64_t last_played = 0;

    int32_t meteor_shower_count = 0;
    int32_t coin_rain = 0;

    std::string world_manifest; // json manifest. TODO: json serialization in io.hpp

private:
    void read_world_flags(io &io, ssize_t &offset) {
        // Game mode
        if (version >= 209) {
            game_mode = io.read<int32_t>(offset);

            world_flags[world_flag::drunk_world] =
                io.read_ge<bool, 222>(offset, version);

            world_flags[world_flag::get_good_world] =
                io.read_ge<bool, 227>(offset, version);

            world_flags[world_flag::tenth_anniversary_world] =
                io.read_ge<bool, 238>(offset, version);

            world_flags[world_flag::dont_starve_world] =
                io.read_ge<bool, 239>(offset, version);

            world_flags[world_flag::not_the_bees_world] =
                io.read_ge<bool, 241>(offset, version);

            world_flags[world_flag::remix_world] =
                io.read_ge<bool, 249>(offset, version);

            world_flags[world_flag::no_traps_world] =
                io.read_ge<bool, 266>(offset, version);

            if (version < 267) {
                world_flags[world_flag::zenith_world] =
                    world_flags[world_flag::remix_world] &&
                    world_flags[world_flag::drunk_world];
            } else {
                world_flags[world_flag::zenith_world] =
                    io.read<bool>(offset);
            }

            world_flags[world_flag::skyblock_world] =
                io.read_ge<bool, 302>(offset, version);
        } else {
            if (version >= 112)
                game_mode = io.read_ge<bool, 112>(offset, version) ? 1 : 0;

            if (version == 208 && io.read<bool>(offset))
                game_mode = 2;
        }

        creation_time =
            io.read_ge<int64_t, 141>(offset, version);

        last_played =
            io.read_ge<int64_t, 284>(offset, version);

        moon_type = io.read<uint8_t>(offset);

        offset += io.read(tree_x);
        offset += io.read(tree_style);

        offset += io.read(cave_back_x);
        offset += io.read(cave_back_style);

        ice_back_style = io.read<int32_t>(offset);
        jungle_back_style = io.read<int32_t>(offset);
        hell_back_style = io.read<int32_t>(offset);

        spawn_tile.x = io.read<int32_t>(offset);
        spawn_tile.y = io.read<int32_t>(offset);

        world_surface = static_cast<int16_t>(
            io.read<double>(offset)
        );

        rock_layer = static_cast<int16_t>(
            io.read<double>(offset)
        );

        time = static_cast<int32_t>(
            io.read<double>(offset)
        );

        time_flags[time_flag::day_time] =
            io.read<bool>(offset);

        moon_phase = static_cast<uint8_t>(
            io.read<int32_t>(offset)
        );

        time_flags[time_flag::blood_moon] =
            io.read<bool>(offset);

        time_flags[time_flag::eclipse] =
            io.read<bool>(offset);

        dungeon.x = io.read<int32_t>(offset);
        dungeon.y = io.read<int32_t>(offset);

        world_flags[world_flag::crimson] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_boss_1] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_boss_2] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_boss_3] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_queen_bee] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_mech_boss_1] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_mech_boss_2] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_mech_boss_3] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_mech_boss_any] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_plant_boss] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_golem_boss] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_slime_king] =
            io.read_ge<bool, 118>(offset, version);

        saved_goblin = io.read<bool>(offset);
        saved_wizard = io.read<bool>(offset);
        saved_mech = io.read<bool>(offset);

        world_flags[world_flag::downed_goblins] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_clown] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_frost] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_pirates] =
            io.read<bool>(offset);

        world_flags[world_flag::shadow_orb_smashed] =
            io.read<bool>(offset);

        spawn_meteor = io.read<bool>(offset);
        shadow_orb_count = io.read<uint8_t>(offset);
        altar_count = io.read<int32_t>(offset);

        world_flags[world_flag::hard_mode] =
            io.read<bool>(offset);

        after_party_of_doom =
            io.read_ge<bool, 257>(offset, version);

        invasion_delay = io.read<int32_t>(offset);
        invasion_size = io.read<int32_t>(offset);

        invasion_type = static_cast<int8_t>(
            io.read<int32_t>(offset)
        );

        invasion_x = io.read<double>(offset);

        if (version >= 118)
            slime_rain_time = io.read<double>(offset);

        sundial_cooldown =
            io.read_ge<uint8_t, 113>(offset, version);

        raining = io.read<bool>(offset);
        rain_time = io.read<int32_t>(offset);
        max_raining = io.read<float>(offset);

        saved_ore_cobalt = static_cast<int16_t>(
            io.read<int32_t>(offset)
        );

        saved_ore_mythril = static_cast<int16_t>(
            io.read<int32_t>(offset)
        );

        saved_ore_adamantite = static_cast<int16_t>(
            io.read<int32_t>(offset)
        );

        for (auto &background : background_styles)
            background = io.read<uint8_t>(offset);

        if (version >= 95) {
            auto angler_count = io.read<int32_t>(offset);

            for (int32_t i = 0; i < angler_count; ++i)
                io.read<std::string>(offset);

            saved_angler =
                io.read_ge<bool, 99>(offset, version);

            angler_quest =
                io.read_ge<int32_t, 101>(offset, version);

            saved_stylist =
                io.read_ge<bool, 104>(offset, version);

            saved_tax_collector =
                io.read_ge<bool, 129>(offset, version);

            saved_golfer =
                io.read_ge<bool, 201>(offset, version);
        }

        if (version < 107) {
            if (invasion_type > 0 && invasion_size > 0) {
                // FakeLoadInvasionStart
            } else {
                offset += io.read<int32_t>(offset);
            }
        }

        if (version >= 108)
            offset += io.read<int32_t>(offset);
        else
            offset += 4; // 86400

        if (version < 109)
            return;

        // BannerSystem.Load(reader, version)
        //
        // Its exact serialized representation is separate from the
        // fields represented by this class.

        if (version < 128)
            return;

        world_flags[world_flag::fast_forward_time_to_dawn] =
            io.read<bool>(offset);

        if (version < 131)
            return;

        world_flags[world_flag::downed_fishron] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_martians] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_ancient_cultist] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_moonlord] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_halloween_king] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_halloween_tree] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_christmas_ice_queen] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_christmas_santank] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_christmas_tree] =
            io.read<bool>(offset);

        if (version < 140)
            return;

        world_flags[world_flag::downed_tower_solar] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_tower_vortex] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_tower_nebula] =
            io.read<bool>(offset);

        world_flags[world_flag::downed_tower_stardust] =
            io.read<bool>(offset);

        tower_active_solar = io.read<bool>(offset);
        tower_active_vortex = io.read<bool>(offset);
        tower_active_nebula = io.read<bool>(offset);
        tower_active_stardust = io.read<bool>(offset);

        lunar_apocalypse_is_up = io.read<bool>(offset);

        if (version < 170) {
            party_manual = false;
            party_genuine = false;
            party_cooldown = 0;
            party_npcs.clear();
        } else {
            party_manual = io.read<bool>(offset);
            party_genuine = io.read<bool>(offset);
            party_cooldown = io.read<int32_t>(offset);

            auto count = io.read<int32_t>(offset);
            party_npcs.resize(count);

            offset += io.read(party_npcs);
        }

        if (version < 174) {
            sandstorm_intended_severity = 0;
        } else {
            const auto sandstorm_happening =
                io.read<bool>(offset);

            const auto sandstorm_time_left =
                io.read<int32_t>(offset);

            const auto sandstorm_severity =
                io.read<float>(offset);

            sandstorm_intended_severity =
                io.read<float>(offset);

            world_flags[world_flag::sandstorm] =
                sandstorm_happening;

            (void)sandstorm_time_left;
            (void)sandstorm_severity;
        }

        // DD2Event.Load(reader, version)
        //
        // The DD2 booleans are represented by world flags.
        //
        // Their exact serialized representation belongs to DD2Event.Load.

        background_styles[8] =
            io.read_gt<uint8_t, 194>(offset, version);

        background_styles[9] =
            io.read_ge<uint8_t, 215>(offset, version);

        if (version > 195) {
            background_styles[10] = io.read<uint8_t>(offset);
            background_styles[11] = io.read<uint8_t>(offset);
            background_styles[12] = io.read<uint8_t>(offset);
        }

        world_flags[world_flag::combat_book_was_used] =
            io.read_ge<bool, 204>(offset, version);

        if (version < 207) {
            lantern_cooldown = 0;
            lantern_genuine = false;
            lantern_manual = false;
            next_night_is_genuine = false;
        } else {
            lantern_cooldown = io.read<int32_t>(offset);
            lantern_genuine = io.read<bool>(offset);
            lantern_manual = io.read<bool>(offset);
            next_night_is_genuine = io.read<bool>(offset);

            world_flags[world_flag::manual_lanterns] =
                lantern_manual;
        }

        // TreeTops.Load(reader, version)

        world_flags[world_flag::force_halloween_for_today] =
            io.read_ge<bool, 212>(offset, version);

        world_flags[world_flag::force_xmas_for_today] =
            io.read_ge<bool, 212>(offset, version);

        saved_ore_copper =
            static_cast<int16_t>(
                io.read_ge<int32_t, 216>(offset, version)
            );

        saved_ore_iron =
            static_cast<int16_t>(
                io.read_ge<int32_t, 216>(offset, version)
            );

        saved_ore_silver =
            static_cast<int16_t>(
                io.read_ge<int32_t, 216>(offset, version)
            );

        saved_ore_gold =
            static_cast<int16_t>(
                io.read_ge<int32_t, 216>(offset, version)
            );

        world_flags[world_flag::bought_cat] =
            io.read_ge<bool, 217>(offset, version);

        world_flags[world_flag::bought_dog] =
            io.read_ge<bool, 217>(offset, version);

        world_flags[world_flag::bought_bunny] =
            io.read_ge<bool, 217>(offset, version);

        world_flags[world_flag::downed_empress_of_light] =
            io.read_ge<bool, 223>(offset, version);

        world_flags[world_flag::downed_queen_slime] =
            io.read_ge<bool, 223>(offset, version);

        world_flags[world_flag::downed_deerclops] =
            io.read_ge<bool, 240>(offset, version);

        world_flags[world_flag::unlocked_slime_blue_spawn] =
            io.read_ge<bool, 250>(offset, version);

        if (version >= 251) {
            bought_merchant = io.read<bool>(offset);
            bought_demolitionist = io.read<bool>(offset);
            bought_party_girl = io.read<bool>(offset);
            bought_dye_trader = io.read<bool>(offset);

            world_flags[world_flag::unlocked_truffle_spawn] =
                io.read<bool>(offset);

            bought_arms_dealer = io.read<bool>(offset);
            bought_nurse = io.read<bool>(offset);
            bought_princess = io.read<bool>(offset);
        }

        world_flags[world_flag::combat_book_volume_two_was_used] =
            io.read_ge<bool, 259>(offset, version);

        world_flags[world_flag::peddlers_satchel_was_used] =
            io.read_ge<bool, 260>(offset, version);

        world_flags[world_flag::unlocked_slime_green_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_old_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_purple_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_rainbow_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_red_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_yellow_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::unlocked_slime_copper_spawn] =
            io.read_ge<bool, 261>(offset, version);

        world_flags[world_flag::fast_forward_time_to_dusk] =
            io.read_ge<bool, 264>(offset, version);

        moondial_cooldown =
            io.read_ge<uint8_t, 264>(offset, version);

        world_flags[world_flag::force_halloween_forever] =
            io.read_ge<bool, 287>(offset, version);

        world_flags[world_flag::force_xmas_forever] =
            io.read_ge<bool, 287>(offset, version);

        world_flags[world_flag::vampire_seed] =
            io.read_ge<bool, 288>(offset, version);

        world_flags[world_flag::infected_seed] =
            io.read_ge<bool, 296>(offset, version);

        if (version >= 291) {
            meteor_shower_count = io.read<int32_t>(offset);
            coin_rain = io.read<int32_t>(offset);
        }

        world_flags[world_flag::team_based_spawns_seed] =
            io.read_ge<bool, 297>(offset, version);

        if (version >= 297) {
            const auto count = io.read<uint8_t>(offset);
            extra_spawn_points.resize(count);

            for (auto &point : extra_spawn_points) {
                point.x = io.read<int16_t>(offset);
                point.y = io.read<int16_t>(offset);
            }
        }

        world_flags[world_flag::dual_dungeons_seed] =
            io.read_ge<bool, 304>(offset, version);

        world_flags[world_flag::more_lightning_seed] =
            io.read_ge<bool, 323>(offset, version);

        world_flags[world_flag::no_lightning_seed] =
            io.read_ge<bool, 323>(offset, version);

        if (version >= 299 && version < 313)
            offset += io.read<uint32_t>(offset);

        if (version < 299)
            world_manifest.clear();
        else
            offset += io.read(world_manifest);
    }

public:
    ssize_t read_new_format(io &io) {
        ssize_t offset{};

        auto meta =
            io.read<file::metadata<file::type::world>>(offset);

        auto nsections = io.read<int16_t>(offset);

        std::vector<int32_t> sections(nsections);
        offset += io.read(sections);

        auto nimportance = io.read<int16_t>(offset);

        bitset<0> importance;
        importance.resize(nimportance);
        offset += io.read(importance);

        offset += io.read(name);

        if (version >= 179) {
            if (version == 179) {
                auto seed_version = io.read<int32_t>(offset);
                seed_text = std::to_string(seed_version);
            } else {
                offset += io.read(seed_text);
            }

            offset += io.read(world_generator_version);
        }

        if (version >= 180)
            offset += io.read(unique_id);

        id = io.read<int32_t>(offset);

        offset += io.read(world_bounds);
        offset += io.read(max_tiles);

        read_world_flags(io, offset);

        return offset;
    }

    ssize_t read_old_format(io &) {
        throw file::file_error{"unimplemented"};
    }

    ssize_t read(io &io) {
        try {
            ssize_t offset{};

            version = io.read<int32_t>(offset);

            if (version <= 0)
                throw file::file_error{"unknown world version"};

            if (version > proto::version)
                throw file::file_error{"world version is too recent"};

            if (version >= 88)
                return offset + read_new_format(io);

            return offset + read_old_format(io);
        } catch (const io::eof &) {
            throw file::file_error{"invalid file"};
        }
    }
};

}
