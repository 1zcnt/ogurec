#pragma once

#include <unordered_map>

#include "../io.hpp"

namespace lz::game::file {

struct file_error : std::exception {
    std::string msg;
    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }

    file_error(const std::string &msg) : msg{msg} {};
};

enum class type : uint8_t {
    none,
    map,
    world,
    player,
};

template<type T>
struct metadata {
    static constexpr uint64_t magic      = 27981915666277746ULL; // relogic string
    static constexpr uint64_t magic_mask = 0x00FFFFFFFFFFFFFFULL;

    uint32_t    revision;
    uint64_t    favorite;

    ssize_t write(io &io) const {
        ssize_t offset{};

        offset += io.write(magic | static_cast<uint64_t>(T) << 56);
        offset += io.write(revision);
        offset += io.write(favorite);
        return offset;
    }

    ssize_t read(io &io) {
        ssize_t offset{};

        auto magic_ = io.read<uint64_t>(offset);
        if ((magic_ & magic_mask) != magic)
            throw file_error{"Invalid file"};
        if (auto t = static_cast<type>(magic_ >> 56); t != T)
            throw file_error{std::format("Unexpected file type {}. Expected {} file",
                misc::enum_to_string(t),
                misc::enum_to_string(T))};
        revision = io.read<uint32_t>(offset);
        favorite = io.read<uint64_t>(offset);
        return offset;
    }
};

}
