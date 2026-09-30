#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "io.hpp"

namespace lz {
namespace encoders {
template <std::integral PrefixType>
struct [[=io::encoder_impl]] prefixed_vector {
    template<typename T, misc::container C> requires std::same_as<typename C::value_type, T>
    static ssize_t read(io &io, C &v) {
        PrefixType size;
        auto offset = io.read(size);

        v.clear();
        v.resize(size);

        return offset + io.read(v);
    }
    template<typename T, misc::container C> requires std::same_as<typename C::value_type, T>
    static ssize_t write(io &io, C &v) {
        auto offset = io.write(static_cast<PrefixType>(v.size()));
        return offset + io.write(v);
    }
};

struct [[=io::encoder_impl]] nullterm_vector {
    template<misc::container C> requires misc::numeric<typename C::value_type>
    static ssize_t read(io &io, C &v) {
        ssize_t offset = 0;
        v.clear();
        while (true) {
            typename C::value_type e;
            offset += io.read(e);
            if (e == 0) break;
            v.push_back(e);
        }
        return offset;
    }

    template<misc::container C> requires misc::numeric<typename C::value_type>
    static ssize_t write(io &io, const C &v) {
        auto offset = io.write(v);
        offset += io.write(C::value_type(0));
        return offset;
    }
};
}

struct nstring {
    enum type : uint8_t {
        Literal,
        Formattable,
        LocalizationKey,
    } type;

    std::string text;
    [[=encoders::prefixed_vector<uint8_t>{}]]
    std::vector<std::string> substitutions;

    nstring(const char* str)
    : type(Literal)
    , text(str)
    {}

    nstring(const std::string_view str)
    : nstring(str.data())
    {}

    nstring(const std::string& str)
    : nstring(str.c_str())
    {}
};

template <misc::numeric T>
struct vec2 {
    T x, y;
};

template <misc::numeric T>
struct vec3 {
    T x, y, z;
};

template <misc::numeric T>
struct vec4 {
    T x, y, z, w;
};

template <misc::numeric T>
struct rect {
    T x1, x2, y1, y2;
};

using rgb = vec3<uint8_t>;

}