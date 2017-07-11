#pragma once

#include <mbgl/util/type_list.hpp>

#include <type_traits>
#include <tuple>

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

    using std::tuple<Ts...>::tuple;

    template <class I>
    auto& get() {
        return std::get<TypeIndex<I, Is...>::value>(*this);
    }

    template <class I>
    const auto& get() const {
        return std::get<TypeIndex<I, Is...>::value>(*this);
    }

    template <class... Js, class... Us>
    IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>>
    concat(const IndexedTuple<TypeList<Js...>, TypeList<Us...>>& other) const {
        return IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>> {
            get<Is>()...,
            other.template get<Js>()...
        };
    }
};

} // namespace mbgl
