#pragma once

#include <mbgl/util/type_list.hpp>

#include <tuple>
#include <type_traits>

namespace mbgl {

template <class T, class... Ts>
struct TypeIndex;

template <class T, class... Ts>
struct TypeIndex<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <class T, class U, class... Ts>
struct TypeIndex<T, U, Ts...> : std::integral_constant<std::size_t, 1 + TypeIndex<T, Ts...>::value> {};

template <class...> class IndexedTuple;

// A tuple of Ts, where individual members can be accessed via `t.get<I>()` for I ∈ Is.
//
// See https://github.com/mapbox/cpp/blob/master/C%2B%2B%20Structural%20Metaprogramming.md
// for motivation.
//
template <class... Is, class... Ts>
class IndexedTuple<TypeList<Is...>, TypeList<Ts...>> : public std::tuple<Ts...> {
public:
    static_assert(sizeof...(Is) == sizeof...(Ts), "IndexedTuple size mismatch");

    template <class I>
    auto& get() {
        return std::get<TypeIndex<I, Is...>::value, Ts...>(*this);
    }

    template <class I>
    const auto& get() const {
        return std::get<TypeIndex<I, Is...>::value, Ts...>(*this);
    }

    template <class... Us>
    IndexedTuple(Us&&... other) : std::tuple<Ts...>(std::forward<Us>(other)...) {};

    template <class... Js, class... Us>
    IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>>
    concat(const IndexedTuple<TypeList<Js...>, TypeList<Us...>>& other) const {
        return IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>> {
            get<Is>()...,
            other.template get<Js>()...
        };
    }

    // Help out MSVC++
    bool operator==(const IndexedTuple<TypeList<Is...>, TypeList<Ts...>>& other) const {
        return static_cast<const std::tuple<Ts...>&>(*this) == static_cast<const std::tuple<Ts...>&>(other);
    }

    bool operator!=(const IndexedTuple<TypeList<Is...>, TypeList<Ts...>>& other) const {
        return !(*this == other);
    }
};

template <class, class T>
using ExpandToType = T;

} // namespace mbgl
