#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

namespace lz {
constexpr size_t bits_ceil(size_t bits)
{
    return bits / 8 + (bits % 8 > 0 ? 1 : 0);
}

template <auto flags, size_t flags_casted = static_cast<size_t>(flags), auto size = bits_ceil(flags_casted)>
struct bitset : std::array<std::uint8_t, size> {
    using array_type = std::array<std::uint8_t, size>;

private:
    struct bit_ref {
        bitset<flags>& a;
        int referenced_byte;
        uint8_t referenced_bit_mask;

        bool operator=(bool b)
        {
            if (b)
                a.at(referenced_byte) |= referenced_bit_mask;
            else
                a.at(referenced_byte) &= ~referenced_bit_mask;
            return b;
        }

        constexpr operator bool() const
        {
            return (a.at(referenced_byte) & referenced_bit_mask) > 0;
        }
    };

public:
    template <class T>
    constexpr bool operator[](T idx) const
    {
        unsigned int i = (unsigned int)(idx);
        assert(i < flags_casted);
        return (array_type::at(i / 8) & (1 << (i % 8))) > 0;
    }

    template <class T>
    bit_ref operator[](T idx)
    {
        unsigned int i = (unsigned int)(idx);
        assert(i < flags_casted);
        return bit_ref(*this, i / 8, (1 << (i % 8)));
    }
};

// FIXME: ??
template <>
struct bitset<0> : std::vector<std::uint8_t> {
    using container_type = std::vector<std::uint8_t>;

private:
    struct bit_ref {
        bitset<0>& a;
        int referenced_byte;
        uint8_t referenced_bit_mask;

        bool operator=(bool b)
        {
            if (b)
                a.at(referenced_byte) |= referenced_bit_mask;
            else
                a.at(referenced_byte) &= ~referenced_bit_mask;
            return b;
        }

        constexpr operator bool() const
        {
            return (a.at(referenced_byte) & referenced_bit_mask) > 0;
        }
    };

public:
    void resize(size_t size) {
        container_type::resize(bits_ceil(size));
    }

    template <class T>
    constexpr bool operator[](T idx) const
    {
        const auto i = static_cast<unsigned int>(idx);
        assert(i < container_type::size());
        return (at(i / 8) & (1 << (i % 8))) > 0;
    }

    template <class T>
    bit_ref operator[](T idx)
    {
        const auto i = static_cast<size_t>(idx);
        assert(i < container_type::size());
        return bit_ref(*this, i / 8, (1 << (i % 8)));
    }
};


}