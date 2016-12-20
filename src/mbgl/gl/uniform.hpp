#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <array>
#include <functional>

namespace mbgl {
namespace gl {

template <class T>
void bindUniform(UniformLocation, const T&);

template <class Tag, class T>
class UniformValue {
public:
    explicit UniformValue(T t_) : t(std::move(t_)) {}
    T t;
};

template <class Tag, class T>
class Uniform {
public:
    using Value = UniformValue<Tag, T>;

    class State {
    public:
        State(UniformLocation location_) : location(std::move(location_)) {}

        void operator=(const Value& value) {
            if (!current || *current != value.t) {
                current = value.t;
                bindUniform(location, value.t);
            }
        }

        UniformLocation location;
        optional<T> current = {};
    };
};

template <class Tag, class T>
using UniformScalar = Uniform<Tag, T>;

template <class Tag, class T, size_t N>
using UniformVector = Uniform<Tag, std::array<T, N>>;

template <class Tag, class T, size_t N>
using UniformMatrix = Uniform<Tag, std::array<T, N*N>>;

#define MBGL_DEFINE_UNIFORM_SCALAR(type_, name_) \
    struct name_ : ::mbgl::gl::UniformScalar<name_, type_> { static constexpr auto name = #name_; }

#define MBGL_DEFINE_UNIFORM_VECTOR(type_, n_, name_) \
    struct name_ : ::mbgl::gl::UniformVector<name_, type_, n_> { static constexpr auto name = #name_; }

#define MBGL_DEFINE_UNIFORM_MATRIX(type_, n_, name_) \
    struct name_ : ::mbgl::gl::UniformMatrix<name_, type_, n_> { static constexpr auto name = #name_; }

UniformLocation uniformLocation(ProgramID, const char * name);

template <class... Us>
class Uniforms {
public:
    using State = IndexedTuple<TypeList<Us...>, TypeList<typename Us::State...>>;
    using Values = IndexedTuple<TypeList<Us...>, TypeList<typename Us::Value...>>;

    static State state(const ProgramID& id) {
        return State(typename Us::State(uniformLocation(id, Us::name))...);
    }

    static std::function<void ()> binder(State& state, Values&& values_) {
        return [&state, values = std::move(values_)] () mutable {
            util::ignore({ (state.template get<Us>() = values.template get<Us>(), 0)... });
        };
    }
};

} // namespace gl
} // namespace mbgl
