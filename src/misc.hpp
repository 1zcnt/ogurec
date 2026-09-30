#pragma once

#include <bit>
#include <concepts>
#include <ranges>
#include <type_traits>
#include <string_view>
#include <format>
#include <span>
#include <meta>

#include "meta.hpp"

namespace lz::misc {

// types
template <typename T>
concept numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept numeric_enum =
    std::is_enum_v<T> &&
    numeric<std::underlying_type_t<T>>;

template <class T>
concept container = requires(T& t) {
    { t.data() };
    { t.size() };
    { t.begin() };
    { t.end() };
};

// bits
template<std::integral T>
T swap(T v) { return std::byteswap(v); }

template<std::floating_point T>
T swap(T v) {
    using U = std::conditional_t<
        sizeof(T) == sizeof(std::uint32_t),
        std::uint32_t,
        std::uint64_t
    >;
    return std::bit_cast<T>(std::byteswap(std::bit_cast<U>(v)));
}

template <numeric T>
constexpr T to_little(T v) {
    if constexpr (std::endian::native == std::endian::little)
        return v;
    else return swap(v);
}

constexpr uint64_t hash(std::string_view s) {
    uint64_t h = 14695981039346656037ull; // FNV-1a offset
    for (const auto c : s)
        h = (h ^ static_cast<unsigned char>(c)) * 1099511628211ull;
    return h;
}

consteval uint64_t hash(meta::info m) {
    return hash(meta::display_string_of(m));
}

consteval uint64_t rand_impl(meta::info ns, meta::info m, std::string_view file, uint64_t line) {
    return hash(std::string_view(__TIME__)) ^ hash(ns) ^ hash(m) ^ hash(file) ^ line;
}

#define const_rand() [] {             \
      struct {} line ## __LINE__ ## cnt ## __COUNTER__ {};  \
      return lz::misc::rand_impl(meta::current_namespace(), ^^line ## __LINE__ ## cnt ## __COUNTER__, __FILE__, __LINE__ * __COUNTER__); }()

// span of static storage usable as a template argument (std::span isn't structural)
template <typename T>
struct struct_span {
    const T *ptr;
    std::size_t len;

    consteval struct_span(std::span<const T> s) : ptr(s.data()), len(s.size()) {}

    constexpr const T *begin() const { return ptr; }
    constexpr const T *end() const { return ptr + len; }
    constexpr std::size_t size() const { return len; }
    constexpr const T &operator[](std::size_t i) const { return ptr[i]; }
};

// other

inline std::string hex_dump(const std::span<const uint8_t> data) {
    std::string result;
    result.reserve(data.size() * 3);

    for (uint8_t b : data)
        std::format_to(std::back_inserter(result), "{:02x} ", b);

    if (!result.empty())
        result.pop_back();

    return result;
}
}