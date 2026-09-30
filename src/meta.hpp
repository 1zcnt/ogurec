#pragma once

#include <cstring>
#include <format>
#include <meta>
#include <ranges>

namespace lz::meta {
using namespace std::meta;

constexpr auto ctx = access_context::current();

static constexpr auto nullinfo = info{};

// from gnu libc++
template<typename Rg>
concept statically_sized = requires(Rg&& r) {
    static_cast<char(*)[static_cast<size_t>(std::ranges::size(r)) >= 0]>(nullptr);
};

template <typename ...Ts>
concept same_types = [] {
    constexpr auto N = sizeof...(Ts);
    if constexpr (N < 2) return false;

    template for (constexpr auto i : std::views::iota(size_t{1}, N))
        if constexpr (!std::same_as<Ts...[i-1], Ts...[i]>)
            return false;
    return true;
}();

template <same_types ...Ts> requires (sizeof...(Ts) > 0)
using unify_same = Ts...[0];

template <typename T, typename ...Ts>
concept type_repeated = (std::same_as<Ts, T> && ...);

// template <typename Rg, typename T>
// concept static_range_of =
//      std::ranges::input_range<Rg> &&
//      statically_sized<Rg> &&
//      std::same_as<T, std::ranges::range_value_t<Rg>>;

template <typename A, typename T>
concept array_of = requires (A a, size_t i) {
    {a[i]} -> std::same_as<T>;
};

template <typename Rg, typename T>
concept valid_container_of = array_of<Rg, T>;

consteval auto make_indexes_refl(size_t from, size_t till) {
    std::vector<info> is{};
    for (auto i = from; i < till; ++i)
        is.push_back(reflect_constant(i));
    return substitute(^^std::index_sequence, is);
}

consteval auto make_indexes_refl(size_t till) { return make_indexes_refl(0, till); }

template <typename T, type_repeated<T> ...Ts>
constexpr auto make_data(Ts ...args) {
    static T data[] = { args... };
    return &data[0];
}

template <typename T, valid_container_of<T> ...Rgs>
constexpr auto concat_helper(Rgs... rs) {
    constexpr auto sz = (std::ranges::size(rs) + ...);
    static T arr[sz];

    size_t idx = 0;
    template for (constexpr auto i : std::views::iota(size_t{0}, sizeof...(Rgs)))
        template for (constexpr auto j : std::views::iota(size_t{0}, std::ranges::size(rs...[i])))
            arr[idx++] = rs...[i][j];

    using indexes = [: make_indexes_refl(sz) :];
    return std::pair{arr, indexes{}};
}

static constexpr auto uninitialized_n = std::numeric_limits<size_t>::max();
template <typename T, std::size_t N = uninitialized_n, auto _size = N == uninitialized_n ? 0 : N>
struct array {
    using value_type = T;

    const T* data;

    template <typename ...Ts> requires type_repeated<T, Ts...>
    constexpr array(Ts ...args) : data(make_data<T>(args ...)) {}

    template <valid_container_of<T> R, size_t ...Is>
    constexpr array(std::pair<R, std::index_sequence<Is...>> v)
        : array( v.first[Is]... ) {}

    template <valid_container_of<T> Rg0, valid_container_of<T> ...Rgs>
    constexpr array(Rg0 r, Rgs... rs)
        : array(concat_helper<T>(r, rs...)) {}

    constexpr T& at(ssize_t idx) {
        static_assert(idx < N);
        return data[idx];
    }

    constexpr auto operator[](const ssize_t idx) { return at(idx); }
    constexpr auto begin() { return data; }
    constexpr auto end() { return data + _size; }
    constexpr auto size() { return _size; }
};

template <typename T, type_repeated<T> ...Ts>
array(Ts ...args) -> array<T, sizeof...(Ts)>;

template<std::ranges::input_range Rg>
consteval auto dsa(Rg&& r) { return std::define_static_array(r); }

// FIXME: implement
template <typename... Args>
consteval std::string_view const_format(std::format_string<Args...> fmt, Args&&... args) {
    return "";
}

template <info I, typename F> requires std::is_enum_v<F>
consteval bool check_flag(F f) {
    template for (constexpr auto dm : dsa(annotations_of_with_type(I, ^^F)))
        if (extract<F>(dm) == f)
            return true;
    return false;
}

template <typename T, typename A>
concept not_annotated_with_type = annotations_of_with_type(^^T, ^^A).empty();
template <typename T, typename A>
concept annotated_with_type_once = annotations_of_with_type(^^T, ^^A).size() == 1;

template<info m, info t>
consteval auto annotations_of_with_template() {
    using namespace std::meta;

    std::vector<info> annotations;
    template for (constexpr auto a : dsa(annotations_of(m))) {
        if (template_of(a) == t) {
            annotations.push_back(a);
        }
    }

    return annotations;
}

template <typename T>
concept enum_type = std::is_enum_v<T>;

// example from from p2996
template<enum_type E>
constexpr std::string_view enum_to_string(E value) {
        template for (constexpr auto e : dsa(enumerators_of(^^E)))
            if (value == [:e:]) return identifier_of(e);
    return "<unnamed>";
}

}
