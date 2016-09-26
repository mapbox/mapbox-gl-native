#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/util/optional.hpp>

#include <array>
#include <functional>
#include <tuple>

namespace mbgl {
namespace gl {

template <class T>
void bindUniform(UniformLocation, const T&);

template <class Tag, class T>
class Uniform {
public:
    class Value {
    public:
        Value(T t_) : t(std::move(t_)) {}
        T t;
    };

    class State {
    public:
        State(const char* name, const Shader& shader)
            : location(shader.getUniformLocation(name)) {}

        void operator=(const Value& value) {
            if (!current || *current != value.t) {
                current = value.t;
                bindUniform(location, value.t);
            }
        }

    private:
        optional<T> current;
        UniformLocation location;
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

template <class... Us>
class Uniforms {
public:
    using State = std::tuple<typename Us::State...>;
    using Values = std::tuple<typename Us::Value...>;

    static State state(const Shader& shader) {
        return State { { Us::name, shader }... };
    }

    template <class... Args>
    static Values values(Args&&... args) {
        return Values { std::forward<Args>(args)... };
    }

    static std::function<void ()> binder(State& state, Values&& values_) {
        return [&state, values = std::move(values_)] () mutable {
            noop((std::get<typename Us::State>(state) = std::get<typename Us::Value>(values), 0)...);
        };
    }

private:
    // This exists only to provide a varags context for unpacking the assignments in `binder`.
    template <int...> static void noop(int...) {}
};

} // namespace gl
} // namespace mbgl
