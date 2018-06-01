#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/indexed_tuple.hpp>

#include <array>
#include <vector>
#include <map>
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

class ActiveUniform {
public:
    std::size_t size;
    UniformDataType type;
};

#ifndef NDEBUG

template <class T>
bool verifyUniform(const ActiveUniform&);

using ActiveUniforms = std::map<std::string, ActiveUniform>;
ActiveUniforms activeUniforms(ProgramID);

#endif

template <class Tag, class T>
class Uniform {
public:
    using Value = UniformValue<Tag, T>;

    using Type = T;

    class State {
    public:
        State(UniformLocation location_) : location(std::move(location_)) {}

        void operator=(const Value& value) {
            if (location >= 0 && (!current || *current != value.t)) {
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
    struct name_ : ::mbgl::gl::UniformScalar<name_, type_> { static auto name() { return #name_; } }

#define MBGL_DEFINE_UNIFORM_VECTOR(type_, n_, name_) \
    struct name_ : ::mbgl::gl::UniformVector<name_, type_, n_> { static auto name() { return #name_; } }

#define MBGL_DEFINE_UNIFORM_MATRIX(type_, n_, name_) \
    struct name_ : ::mbgl::gl::UniformMatrix<name_, type_, n_> { static auto name() { return #name_; } }

UniformLocation uniformLocation(ProgramID, const char * name);

template <class... Us>
class Uniforms {
public:
    using Types = TypeList<Us...>;
    using State = IndexedTuple<TypeList<Us...>, TypeList<typename Us::State...>>;
    using Values = IndexedTuple<TypeList<Us...>, TypeList<typename Us::Value...>>;
    using NamedLocations = std::vector<std::pair<const std::string, UniformLocation>>;

    static State bindLocations(const ProgramID& id) {
#ifndef NDEBUG
        // Verify active uniform types match the enum
        const auto active = activeUniforms(id);

        util::ignore(
            { // Some shader programs have uniforms declared, but not used, so they're not active.
              // Therefore, we'll only verify them when they are indeed active.
              (active.find(Us::name()) != active.end()
                   ? verifyUniform<typename Us::Type>(active.at(Us::name()))
                   : false)... });
#endif

        return State { { uniformLocation(id, Us::name()) }... };
    }

    template <class Program>
    static State loadNamedLocations(const Program& program) {
        return State(typename Us::State(program.uniformLocation(Us::name()))...);
    }

    static NamedLocations getNamedLocations(const State& state) {
        return NamedLocations{ { Us::name(), state.template get<Us>().location }... };
    }

    static void bind(State& state, const Values& values) {
        util::ignore({ (state.template get<Us>() = values.template get<Us>(), 0)... });
    }
};

template <class... Us>
using ConcatenateUniforms = typename TypeListConcat<typename Us::Types...>::template ExpandInto<Uniforms>;

} // namespace gl
} // namespace mbgl
