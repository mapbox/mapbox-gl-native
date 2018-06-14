#pragma once

#include <mbgl/util/type_list.hpp>
#include <mbgl/util/tuple.hpp>

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
class IndexedTuple<TypeList<Is...>, TypeList<Ts...>> : public tuple_polyfill<Ts...> {
public:
    static_assert(sizeof...(Is) == sizeof...(Ts), "IndexedTuple size mismatch");

    template <class I>
    auto& get() {
        return get_polyfill<TypeIndex<I, Is...>::value>(*this);
    }

    template <class I>
    const auto& get() const {
        return get_polyfill<TypeIndex<I, Is...>::value>(*this);
    }

    template <class... Us>
    IndexedTuple(Us&&... other) : tuple_polyfill<Ts...>(std::forward<Us>(other)...) {}

    template <class... Js, class... Us>
    IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>>
    concat(const IndexedTuple<TypeList<Js...>, TypeList<Us...>>& other) const {
        return IndexedTuple<TypeList<Is..., Js...>, TypeList<Ts..., Us...>> {
            get<Is>()...,
            other.template get<Js>()...
        };
    }
};

template <class, class T>
using ExpandToType = T;

} // namespace mbgl
