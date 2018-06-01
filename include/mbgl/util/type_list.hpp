#pragma once

#include <type_traits>
#include <tuple>

namespace mbgl {

template <class... Ts>
struct TypeList {
    template <template <class...> class T, class... Ps>
    using ExpandInto = T<Ps..., Ts...>;
};

namespace detail {

template <class, class>
struct TypeCons;

template <class T, class... Ts>
struct TypeCons<T, TypeList<Ts...>> {
    using Type = TypeList<T, Ts...>;
};

template <class, template <class> class>
struct TypeFilter;

template <template <class> class Predicate>
struct TypeFilter<TypeList<>, Predicate> {
    using Type = TypeList<>;
};

template <template <class> class Predicate, class T, class... Ts>
struct TypeFilter<TypeList<T, Ts...>, Predicate> {
    using Tail = typename TypeFilter<TypeList<Ts...>, Predicate>::Type;
    using Type = std::conditional_t<Predicate<T>::value, typename TypeCons<T, Tail>::Type, Tail>;
};

template <class...>
struct TypeListConcat;

template <>
struct TypeListConcat<> {
    using Type = TypeList<>;
};

template <class... As>
struct TypeListConcat<TypeList<As...>> {
    using Type = TypeList<As...>;
};

template <class... As, class... Bs, class... Rs>
struct TypeListConcat<TypeList<As...>, TypeList<Bs...>, Rs...> {
    using Type = typename TypeListConcat<TypeList<As..., Bs...>, Rs...>::Type;
};

} // namespace detail

template <class TypeList, template <class> class Predicate>
using FilteredTypeList = typename detail::TypeFilter<TypeList, Predicate>::Type;

template <class... Ts>
using TypeListConcat = typename detail::TypeListConcat<Ts...>::Type;

} // namespace mbgl
