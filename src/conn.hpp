#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <print>
#include <unordered_map>
#include <variant>
#include <netinet/in.h>

#include "io.hpp"
#include "proto.hpp"

namespace lz {
namespace detail {
using namespace std::meta;

template <typename T> using ss = misc::struct_span<T>;

static constexpr auto ctx = access_context::current();

struct temp_ns_type {
    bool global = false;
    uint64_t hash = 0;

    template <std::size_t N>
    constexpr temp_ns_type(const char (&str)[N]) {
        if (N >= 2 && str[0] == ':' && str[1] == ':') {
            global = true;
        }
        hash = misc::hash(str);
    }

    temp_ns_type() = default;
};

struct handler_annotation {
    int priority = 0;
    bool final = false;
    temp_ns_type ns;
};

using middleware = std::function<bool()>;
using handler_proc_type    = bool (*)(void*, const std::vector<middleware>&, std::span<uint8_t>);

struct handler_info {
    handler_proc_type proc;
    std::span<const netmessage::message_id> args;
    void *self;
    uint64_t ns;

    int priority;
    bool final;

    // debug
    std::string_view name;

    auto operator()(const auto &m, auto p) const {
        return proc(self, m, p);
    }
};


template<auto P>
consteval bool check_params() {
    template for (constexpr auto p: std::define_static_array(P))
            if constexpr (netmessage::netmessage<typename [:type_of(p):]> == false)
                return false;
    return true;
}

template<auto P>
consteval handler_annotation get_handler_annotation()
    requires (annotations_of_with_type(P, ^^handler_annotation).size() == 1) {
    return extract<handler_annotation>(annotations_of_with_type(P, ^^handler_annotation)[0]);
}

template<typename T>
consteval auto handlers_of() {
    constexpr auto M = ^^T;
    std::vector<info> handlers;
    static constexpr auto members =
            std::define_static_array(members_of(M, ctx));
    template for (constexpr auto m: members) {
        if constexpr (is_function(m)) {
            if (annotations_of_with_type(m, ^^handler_annotation).size() != 1)
                continue;
            static constexpr auto params = std::define_static_array(parameters_of(m));
            if (!check_params<ss(params)>())
                continue;
            handlers.push_back(m);
        }
    }
    return handlers;
}

template<auto Ms, std::size_t... I>
using tuplify = std::tuple<
    typename [:type_of(Ms[I]):]...
>;

template<typename T, auto F, std::size_t... I> requires (sizeof...(I) > 0)
consteval handler_proc_type wrap_function(std::index_sequence<I...>) {
    static constexpr auto params = std::define_static_array(
        parameters_of(F));
    using args_t = tuplify<ss(params), I...>;

    return [](void *self, const std::vector<middleware> &ms, const std::span<uint8_t> payload) {
        args_t args;

        auto ok = std::apply([ms, payload](auto &... args_) {
            buffered_io io(payload);
            auto offset = (io.read(args_) + ...);
            assert(offset == payload.size());

            return std::ranges::all_of(ms, [] (const auto&m) {return m(); });
        }, args);

        if (!ok) return false;

        const auto self_ = static_cast<T *>(self);
        self_->[:F:](std::get<I>(args)...);
        return true;
    };
}

template<auto P, size_t... I>
consteval auto handler_args_ids(std::index_sequence<I...>) {
    return std::define_static_array(std::vector{
        netmessage::id<typename [:type_of(P[I]):]>...
    });
}

template <typename T, auto F>
consteval handler_info wrap_handler() {
    constexpr auto params = std::define_static_array(parameters_of(F));
    constexpr auto indices = std::make_index_sequence<params.size()>{};

    auto ns = get_handler_annotation<F>().ns;
    auto class_name = display_string_of(^^T);

    if (!ns.global)
        ns.hash ^= misc::hash(class_name.data());

    return {
        .proc       = wrap_function<T, F>(indices),
        .args       = handler_args_ids<ss(params)>(indices),
        .ns         = ns.hash,
        .priority   = get_handler_annotation<F>().priority,
        .final      = get_handler_annotation<F>().final,
        .name       = display_string_of(F),
    };
}

template <typename T, auto info_list, std::size_t... I>
consteval auto wrap_handlers(std::index_sequence<I...>) {
    return std::array<handler_info, sizeof...(I)>{
        wrap_handler<T, info_list[I]>()...
    };
}

template<typename T>
consteval auto make_handlers_ct() {
    constexpr auto static ms = std::define_static_array(
        handlers_of<T>());
    return wrap_handlers<T, ss(ms)>(std::make_index_sequence<ms.size()>{});
}

template<typename T>
auto make_handlers_rt(T *self) {
    auto handlers = make_handlers_ct<T>();
    for (auto &e : handlers)
        e.self = self;
    return handlers;
}

}

class conn {
    file_io fd;

    sockaddr_in conn_addr;
    socklen_t conn_addr_size;

    static constexpr auto debug = true;

    std::vector<detail::handler_info> handlers;
    std::unordered_map<uint64_t, std::vector<detail::middleware>> middlewares;

    bool handler_exists(std::span<netmessage::message_id> ids) {
        const auto x = std::ranges::find_if(handlers, [ids] (const auto &h) { return std::ranges::starts_with(h.args, ids); });
        return x != handlers.end();
    }

    std::optional<detail::handler_info> match_ids(std::span<netmessage::message_id> ids) {
        const auto x = std::ranges::find_if(handlers, [ids] (const auto &h) { return std::ranges::equal(ids, h.args);});
        if (x != handlers.end())
            return *x;
        return std::nullopt;
    }

public:
    explicit conn(const int fd, const sockaddr_in conn_addr, const socklen_t conn_addr_size)
        : fd(fd)
          , conn_addr(conn_addr)
          , conn_addr_size(conn_addr_size) {
    }

    template<netmessage::netmessage T>
    ssize_t read(T &v) {
        ssize_t offset{};
        using HT = std::pair<proto::header, T>;

        HT packet = fd.read<HT>(offset);
        v = packet.second;

        assert(offset == packet.first.len);
        return offset;
    }

    uint8_t read_raw(std::vector<uint8_t> &payload) {
        ssize_t offset{};
        auto [size, id] = fd.read<proto::header>(offset);

        payload.resize(size - offset);
        offset += fd.read(payload);

        assert(offset == size);
        return id;
    }

    template<typename T>
    ssize_t write(const T &v) {
        std::vector<uint8_t> payload;
        buffered_io v_io(payload);
        if constexpr (std::same_as<T, netmessage::tile_block>) {
            std::println("compressed: {}", io_impl::compressed_type<T>);
        }
        v_io.write(v);

        return write_raw(netmessage::id<T>, payload);
    }

    ssize_t write_raw(const uint8_t id, const std::span<uint8_t> payload) {
        using T = std::pair<proto::header, std::span<uint8_t> >;
        return fd.write(T({
                              .len = static_cast<uint16_t>(3+payload.size()),
                              .id = id
                          }, payload));
    }

    bool handle() {
        std::vector<netmessage::message_id> ids;
        std::vector<uint8_t> payload;

        do {
            ssize_t offset;
            auto [size, id] = fd.read<proto::header>(offset);
            const auto s = payload.end();


            payload.resize(payload.size() + size-offset);
            std::span mem(payload.end() - size + 3, payload.end());
            offset += fd.read(mem);
            assert(offset == size);


            // FIXME: read netmodules
            if (id == 82) {
                std::println("skipped a netmodule\npayload: [{}]", misc::hex_dump(payload));
                return true;
            }

            ids.emplace_back(id);
            if (const auto h = match_ids(ids); h) {
                if (!(*h)(middlewares[h->ns],payload))
                    break;
                return true;
            }
        } while (handler_exists(ids));

        std::println(std::cerr, "could not find handler for ids {}\npayload: [{}]", ids, misc::hex_dump(payload));
        return false;
    }

    using handler = detail::handler_annotation;

    template<typename T>
    void register_handlers(T &t) { return register_handlers(&t); }

    template<typename T>
    void register_handlers(T *t) {
        auto h = detail::make_handlers_rt(t);
        handlers.insert(handlers.end(), h.begin(), h.end());
    }

    using middleware = detail::middleware;
    template<typename T>
    void register_middleware(std::string_view ns, middleware &&f) {
        auto ns_ = misc::hash(ns);
        if (!ns.starts_with("::"))
            ns_ ^= misc::hash(std::meta::display_string_of(^^T));

        middlewares[ns_].push_back(std::move(f));
    }

    void register_middleware(std::string_view ns, middleware &&f) {
        if (!ns.starts_with("::"))
            return register_middleware<void>(std::format("::{}", ns), std::move(f));
        return register_middleware<void>(ns, std::move(f));
    }
};
}
