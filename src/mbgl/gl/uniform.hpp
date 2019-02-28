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

template <class Value>
class UniformState {
public:
    UniformState(UniformLocation location_) : location(std::move(location_)) {
    }

    void operator=(const Value& value) {
        if (location >= 0 && (!current || *current != value)) {
            current = value;
            bindUniform(location, value);
        }
    }

    UniformLocation location;
    optional<Value> current = {};
};

UniformLocation uniformLocation(ProgramID, const char * name);

template <class>
class Uniforms;

template <class... Us>
class Uniforms<TypeList<Us...>> final {
public:
    using Types = TypeList<Us...>;
    using State = IndexedTuple<TypeList<Us...>, TypeList<UniformState<typename Us::Value>...>>;
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
                   ? verifyUniform<typename Us::Value>(active.at(Us::name()))
                   : false)... });
#endif

        return State(uniformLocation(id, Us::name())...);
    }

    template <class Program>
    static State loadNamedLocations(const Program& program) {
        return State(UniformState<typename Us::Value>(program.uniformLocation(Us::name()))...);
    }

    static NamedLocations getNamedLocations(const State& state) {
        return NamedLocations{ { Us::name(), state.template get<Us>().location }... };
    }

    static void bind(State& state, const Values& values) {
        util::ignore({ (state.template get<Us>() = values.template get<Us>(), 0)... });
    }
};

} // namespace gl
} // namespace mbgl
