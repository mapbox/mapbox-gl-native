#pragma once

#include <utility>
#include <cstddef>

namespace mbgl {

// Using structs instead of constexpr templates because GCC 4.9 doesn't support these yet.

template <char... Cs>
struct string_literal {
    static constexpr const char chars[] = { Cs..., 0 };

    static constexpr const char* value() {
        return chars;
    }
};

template <char... Cs>
constexpr const char string_literal<Cs...>::chars[];

namespace detail {

using string_fn = const char* (*)();

template <string_fn name, std::size_t... Is>
string_literal<name()[Is]...> to_string_literal(std::index_sequence<Is...>);

constexpr std::size_t string_length(const char* const str, std::size_t len = 0) {
    return str[0] ? string_length(str + 1, len + 1) : len;
}

template <class...>
struct concat_literals;

template <>
struct concat_literals<> {
    static constexpr auto value() {
        return "";
    }
};

template <char... As, char... Bs, class... Rest>
struct concat_literals<string_literal<As...>, string_literal<Bs...>, Rest...> {
    static constexpr auto value() {
        return concat_literals<string_literal<As..., Bs...>, Rest...>::value();
    }
};

template <char... Cs>
struct concat_literals<string_literal<Cs...>> {
    static constexpr auto value() {
        return string_literal<Cs...>::value();
    }
};

} // namespace detail

template <detail::string_fn... str>
struct concat_literals {
    static constexpr auto value() {
        return detail::concat_literals<decltype(detail::to_string_literal<str>(
            std::make_index_sequence<detail::string_length(str())>()))...>::value();
    }
};

} // namespace mbgl
