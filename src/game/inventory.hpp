#pragma once

#include <cstdint>

#include "../bitset.hpp"


namespace lz::game {
struct inventory_slot {
    enum class inventory_slot_flags : int {
        favourite,
        blocked_slot,
    };

    uint16_t id;
    uint16_t stack;
    uint8_t prefix;
    uint16_t type;

    bitset<2> flags;
};
}
