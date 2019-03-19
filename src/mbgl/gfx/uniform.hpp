#pragma once

#include <mbgl/util/type_list.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <array>

#define MBGL_DEFINE_UNIFORM_SCALAR(type_, name_)                                                   \
    struct name_ {                                                                                 \
        using Value = type_;                                                                       \
        static constexpr auto name() {                                                             \
            return #name_;                                                                         \
        }                                                                                          \
    }

#define MBGL_DEFINE_UNIFORM_VECTOR(type_, n_, name_)                                               \
    struct name_ {                                                                                 \
        using Value = std::array<type_, n_>;                                                       \
        static constexpr auto name() {                                                             \
            return #name_;                                                                         \
        }                                                                                          \
    }

#define MBGL_DEFINE_UNIFORM_MATRIX(type_, n_, name_)                                               \
    struct name_ {                                                                                 \
        using Value = std::array<type_, n_ * n_>;                                                  \
        static constexpr auto name() {                                                             \
            return #name_;                                                                         \
        }                                                                                          \
    }

namespace mbgl {
namespace gfx {

template <class>
class UniformValues;

template <class... Us>
class UniformValues<TypeList<Us...>> final
    : public IndexedTuple<TypeList<Us...>, TypeList<typename Us::Value...>> {
    using Base = IndexedTuple<TypeList<Us...>, TypeList<typename Us::Value...>>;

public:
    template <class... Args>
    UniformValues(Args&&... args) : Base(std::forward<Args>(args)...) {
    }
};

} // namespace gfx
} // namespace mbgl
