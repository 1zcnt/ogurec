#pragma once

#include <concepts>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <format>
#include <meta>
#include <utility>
#include <variant>

#include <zlib.h>

#include "bitset.hpp"
#include "misc.hpp"

// Reflection-based binary serializer: every type is read/written by walking its
// members at compile time, with annotations for per-member encoders and flags.
// This is the prototype lzfmt grew out of (see the lzfmt repository); it is to
// be replaced by lzfmt, which also implements conditional fields.

namespace lz {

class io;

namespace io_impl {

using namespace std::meta;

enum class serializer_flags {
    compressed,
    ignore,
    stop,

    encoder_impl,
};

using enum serializer_flags;

template <typename T>
concept compressed_type = meta::check_flag<^^T>(compressed);

// encoders
template <std::meta::info M>
consteval std::optional<std::meta::info> find_encoder() {
    template for (constexpr auto e : std::define_static_array(std::meta::annotations_of(M)))
        if constexpr (constexpr auto t = std::meta::type_of(e); meta::check_flag<t>(encoder_impl))
            return t;
    return std::nullopt;
}

template <auto M>
concept encoder_annotated_m = io_impl::find_encoder<M>().has_value();

template <auto M> requires encoder_annotated_m<M>
using encoder_of = [: io_impl::find_encoder<M>().value() :];

template <auto M> using meta_t = [:std::meta::type_of(M):];
template <auto M>
concept annotated_reader_m = encoder_annotated_m<M> && requires(meta_t<M> &v, io &io) {
    { encoder_of<M>::read(io, v)  } -> std::same_as<ssize_t>;
};

template <auto M>
concept annotated_writer_m = encoder_annotated_m<M> && requires(const meta_t<M> &v, io &io) {
    { encoder_of<M>::write(io, v)  } -> std::same_as<ssize_t>;
};

template <typename T> concept annotated_reader = annotated_reader_m<^^T>;
template <typename T> concept annotated_writer = annotated_writer_m<^^T>;
template <typename T> concept encoder_annotated = encoder_annotated_m<^^T>;


template <typename T>
concept member_reader = requires(T& t, io& io) {
    { t.read(io) } -> std::same_as<ssize_t>;
} and not compressed_type<T>;

template <typename T>
concept member_writer = requires(const T& t, io& io) {
    { t.write(io) } -> std::same_as<ssize_t>;
} and not compressed_type<T>;

template <typename T>
concept data_container = misc::container<T> && std::integral<typename T::value_type>;
template <typename T>
concept object_container = misc::container<T> && !data_container<T>;

template <typename T>
concept seriable_struct = std::is_class_v<T> &&
    !(compressed_type<T> ||
      encoder_annotated<T> ||
      misc::container<T> ||
      member_reader<T> ||
      member_writer<T>);
}


template <io_impl::data_container T>
class buffered_io;

class io {
    virtual ssize_t read_some(char *buf, ssize_t bytes) = 0;
    virtual ssize_t read_data(char *buf, ssize_t bytes) = 0;
    virtual ssize_t write_data(const char *buf, ssize_t bytes) = 0;

    std::vector<uint8_t> buffer{};
    std::vector<uint8_t>::iterator buffer_cursor = buffer.begin();

    ssize_t read_data_buffered(char *buf, const ssize_t bytes) {
        if (buffer_cursor == buffer.end())
            return read_data(buf, bytes);

        const auto available = buffer.end() - buffer_cursor;
        const auto n = std::min(bytes, available);

        std::memcpy(buf, &*buffer_cursor, n);
        buffer_cursor += n;

        if (buffer_cursor == buffer.end()) {
            buffer.erase(buffer.begin(),buffer.end());
            buffer_cursor = buffer.begin();
        }
        return n;
    }

    ssize_t read_7bit_int(uint64_t &v) {
        v = 0;
        int shift = 0;
        ssize_t off = 0;
        uint8_t c;
        while (true) {
            // clang-tidy keeps trying make this function static
            off += this->read(c);
            v |= static_cast<uint64_t>(c & 0x7F) << shift;
            if ((c & 0x80) == 0)
                break;
            shift += 7;
            if (shift >= 64) {
                throw std::runtime_error("read_7bit_int: out of bounds");
            }
        }
        return off;
    }

    ssize_t write_7bit_int(uint64_t v) {
        ssize_t bytes = 0;
        do {
            auto byte = static_cast<uint8_t>(v & 0x7F);
            v >>= 7;

            if (v != 0)
                byte |= 0x80;

            this->write(byte);
            ++bytes;
        } while (v != 0);
        return bytes;
    }

    using zlib_done = std::monostate;
public:
    using enum io_impl::serializer_flags;

    struct exception : std::exception {};
    struct eof : exception {
        [[nodiscard]] const char* what() const noexcept override {
            return "EOF";
        }
    };
    struct zlib_error : exception {
        std::string msg;
        explicit zlib_error(const std::string_view proc, const std::string_view message = "")
            : msg(std::format("zlib: {}{}", proc, message.empty() ? "" : std::format(": {}", message))) {}
    };

    virtual ~io() = default;

    template<typename T>
    T read(ssize_t &offset) {
        T v{};
        offset += read(v);
        return std::move(v);
    }

    template<misc::numeric_enum T>
    ssize_t read(T& v) {
        std::underlying_type_t<T> value;
        const auto bytes = read(value);
        v = static_cast<T>(value);

        return bytes;
    }

    // TODO: different integral types, some terraria types use int32_t as bool lmao
    ssize_t read(bool& v) {
        ssize_t offset{};
        auto iv = read<uint8_t>(offset);
        v = iv != 0;

        return offset;
    }

    template<io_impl::compressed_type T>
    requires meta::not_annotated_with_type<T, zlib_done>
    ssize_t read(T& t) {
        using TZ [[=zlib_done{}]] = [: std::meta::type_of(^^t) :];

        std::array<uint8_t, 1024> in_buffer{};
        std::array<uint8_t, 1024> out_buffer{};

        z_stream zs{};

        if (inflateInit2(&zs, -MAX_WBITS) != Z_OK)
            throw zlib_error("inflateInit2");

        zs.next_in = nullptr;
        zs.avail_in = 0;

        int ret = Z_OK;

        try {
            while (ret != Z_STREAM_END) {
                if (zs.avail_in == 0) {
                    const auto n = read(in_buffer);

                    zs.next_in = in_buffer.data();
                    zs.avail_in = static_cast<uInt>(n);
                }

                zs.next_out = out_buffer.data();
                zs.avail_out = static_cast<uInt>(out_buffer.size());

                ret = inflate(&zs, Z_NO_FLUSH);

                if (ret != Z_OK && ret != Z_STREAM_END)
                    break;

                const auto produced =
                    out_buffer.size() - zs.avail_out;

                buffer.insert(
                    buffer.end(),
                    out_buffer.begin(),
                    out_buffer.begin() + produced
                );
            }

            // anything zlib didn't consume belongs to the next object.
            if (zs.avail_in != 0) {
                buffer.insert(
                    buffer.end(),
                    zs.next_in,
                    zs.next_in + zs.avail_in
                );
            }
        } catch (...) {
            inflateEnd(&zs);
            throw;
        }

        inflateEnd(&zs);

        if (ret != Z_STREAM_END)
            throw zlib_error("inflate", "inflate failed");

        return read<TZ>(t);
    }

    template<io_impl::member_reader T>
    ssize_t read(T &v) { return v.read(*this); }

    template<misc::numeric T>
    ssize_t read(T &v) {
        const auto bytes = read_data_buffered(reinterpret_cast<char *>(&v), sizeof(T));
        v = misc::to_little(v);
        return bytes;
    }

    ssize_t read(std::string &v) {
        uint64_t len;
        auto offset = read_7bit_int(len);

        v.resize(len);
        offset += read_data_buffered(v.data(), static_cast<ssize_t>(len));

        return offset;
    }

    template<io_impl::data_container T>
    ssize_t read(T &v) {
        return read_data_buffered(reinterpret_cast<char*>(v.data()), v.size() * sizeof(typename T::value_type));
    }

    template<io_impl::object_container T>
    ssize_t read(T &v) {
        ssize_t offset = 0;
        for (auto &e : v)
            offset += read(e);
        return offset;
    }

    template<io_impl::seriable_struct T>
    constexpr ssize_t read(T &v) {
        using namespace std::meta;
        static constexpr auto ctx = access_context::current();
        static constexpr auto members = std::define_static_array(nonstatic_data_members_of(^^T, ctx));

        ssize_t offset = 0;
        template for (constexpr auto e : members) {
            if constexpr (io_impl::annotated_reader_m<e>)
                offset += io_impl::encoder_of<e>::read(*this, v.[:e:]);
            else if constexpr (meta::check_flag<e>(stop))
                break;
            else if constexpr (meta::check_flag<e>(ignore))
                continue;
            else
                offset += read(v.[:e:]);
        }
        return offset;
    }

    template<io_impl::compressed_type T>
    requires meta::not_annotated_with_type<T, zlib_done>
    ssize_t write(const T& t) {
        using TZ [[=zlib_done{}]] = [: std::meta::type_of(^^t) :];

        std::vector<uint8_t> input;
        {
            buffered_io tmp(input);
            tmp.template write<TZ>(t);
        }

        z_stream zs{};

        if (deflateInit2(
            &zs,
            Z_DEFAULT_COMPRESSION,
            Z_DEFLATED,
            -MAX_WBITS,
            8,
            Z_DEFAULT_STRATEGY
        ) != Z_OK)
            throw zlib_error("deflateInit2");

        std::array<uint8_t, 1024> output{};

        zs.next_in = input.data();
        zs.avail_in = static_cast<uInt>(input.size());

        ssize_t total = 0;

        try {
            int ret;

            do {
                zs.next_out = output.data();
                zs.avail_out = static_cast<uInt>(output.size());

                ret = deflate(&zs, Z_FINISH);

                if (ret != Z_OK && ret != Z_STREAM_END)
                    throw zlib_error("deflate");

                const auto produced =
                    output.size() - zs.avail_out;

                total += write_data(
                    reinterpret_cast<const char*>(output.data()),
                    static_cast<ssize_t>(produced)
                );
            } while (ret != Z_STREAM_END);
        } catch (...) {
            deflateEnd(&zs);
            throw;
        }

        deflateEnd(&zs);
        return total;
    }

    template<misc::numeric_enum T>
    ssize_t write(const T v) { return write(static_cast<const std::underlying_type_t<T>>(v)); }

    template<io_impl::member_writer T>
    ssize_t write(const T &v) {
        return v.write(*this);
    }

    template<misc::numeric T>
    ssize_t write(const T v) {
        auto lv = misc::to_little(v);
        return write_data(reinterpret_cast<const char *>(&lv), sizeof(T));
    }

    ssize_t write(const std::string &v) {
        auto bytes = write_7bit_int(v.length());
        bytes += write_data(v.data(), static_cast<ssize_t>(v.length()));
        return bytes;
    }

    template<io_impl::data_container T>
    ssize_t write(const T &v) {
        return write_data(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(typename T::value_type));
    }

    template<io_impl::object_container T>
    ssize_t write(const T &v) {
        ssize_t offset = 0;
        for (auto &e : v)
            offset += write(e);
        return offset;
    }

    template<io_impl::seriable_struct T>
    constexpr ssize_t write(const T &v) {
        using namespace std::meta;
        using namespace io_impl;
        using namespace meta;

        static constexpr auto ctx = access_context::current();

        ssize_t offset = 0;
        template for (constexpr auto &e : std::define_static_array(
            nonstatic_data_members_of(^^T, ctx))) {
            if constexpr (annotated_reader_m<e>)
                offset += encoder_of<e>::write(*this, v.[:e:]);
            else if constexpr (check_flag<e>(stop))
                break;
            else if constexpr (!check_flag<e>(ignore))
                offset += write(v.[:e:]);
        }
        return offset;
    }
};

class file_io : public io {
    int fd;

public:
    file_io(const file_io&) = delete;
    file_io& operator=(const file_io&) = delete;

    file_io(file_io&& other) noexcept
        : io(other),
          fd(std::exchange(other.fd, -1))
    {}
    file_io& operator=(file_io&& other) noexcept {
        if (this != &other) {
            io::operator=(other);
            if (fd != -1)
                close(fd);

            fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    explicit file_io(const int fd)
        : fd(fd) {
    }
    explicit file_io(const std::string_view filename)
        : fd(open(filename.data(), O_RDWR)) {
        if (fd < 0)
            throw std::runtime_error(strerror(errno));
    }

    ~file_io() override {
        if (fd != -1)
            close(fd);
    }
private:

    ssize_t read_some(char *buf, const ssize_t size) override {
        if (size == 0) return 0;

        const auto bytes = ::read(fd, buf, size);
        if (bytes == 0) {
            throw eof{};
        }

        return bytes;
    }

    ssize_t read_data(char *buf, const ssize_t size) override {
        ssize_t bytes = 0;

        while (bytes < size)
            bytes += read_some(buf + bytes, size - bytes);

        assert(bytes == size);
        return bytes;
    }

    ssize_t write_data(const char *buf, const ssize_t size) override {
        if (size == 0) return 0;

        const auto bytes = ::write(fd, buf, size);
        if (bytes == 0) {
            throw eof{};
        }

        assert(bytes == size);
        return bytes;
    }
};

template <io_impl::data_container T>
class buffered_io : public io {
    T& buffer;
    T::iterator cursor = buffer.begin();

    ssize_t read_some(char* buf, const ssize_t size) override
    {
        return read_data(buf, size);
    }

    ssize_t read_data(char* buf, const ssize_t size) override {
        if (cursor == buffer.end())
            throw eof{};

        const ssize_t available = buffer.end() - cursor;
        const ssize_t bytes = std::min(size, available);
        memcpy(buf, &*cursor, bytes);
        cursor += bytes;
        return bytes;
    }

    ssize_t write_data(const char* buf, const ssize_t size) override {
        if constexpr (requires {
            buffer.insert(buffer.end(), buf, buf + size);
        }) {
            buffer.insert(buffer.end(), buf, buf + size);
            return size;
        }
        throw eof{};
    }

public:
    explicit buffered_io(T& buffer)
        : buffer(buffer)
    {
    }
};
}
