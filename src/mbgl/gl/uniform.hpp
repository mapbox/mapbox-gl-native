#pragma once

#include <mbgl/gfx/uniform.hpp>
#include <mbgl/gl/types.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/literal.hpp>
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
    UniformState(UniformLocation location_ = -1) : location(location_) {}

    void operator=(const Value& value) {
        if (location >= 0 && (!current || *current != value)) {
            current = value;
            bindUniform(location, value);
        }
    }

    UniformLocation location;
    optional<Value> current = {};
};

UniformLocation uniformLocation(ProgramID, const char* name);

using NamedUniformLocations = std::vector<std::pair<const std::string, UniformLocation>>;

template <class>
class UniformStates;

template <class... Us>
class UniformStates<TypeList<Us...>> final {
private:
    using State = IndexedTuple<TypeList<Us...>, TypeList<UniformState<typename Us::Value>...>>;

    State state;

public:
    void queryLocations(const ProgramID& id) {
#ifndef NDEBUG
        // Verify active uniform types match the enum
        const auto active = gl::activeUniforms(id);

        util::ignore(
            { // Some shader programs have uniforms declared, but not used, so they're not active.
              // Therefore, we'll only verify them when they are indeed active.
              (active.find(concat_literals<&string_literal<'u', '_'>::value, &Us::name>::value()) != active.end()
                   ? verifyUniform<typename Us::Value>(active.at(concat_literals<&string_literal<'u', '_'>::value, &Us::name>::value()))
                   : false)... });
#endif

        state = State{ gl::uniformLocation(id, concat_literals<&string_literal<'u', '_'>::value, &Us::name>::value())... };
    }

    NamedUniformLocations getNamedLocations() const {
        return NamedUniformLocations{ { concat_literals<&string_literal<'u', '_'>::value, &Us::name>::value(), state.template get<Us>().location }... };
    }

    void bind(const gfx::UniformValues<TypeList<Us...>>& values) {
        util::ignore({ (state.template get<Us>() = values.template get<Us>(), 0)... });
    }
};

} // namespace gl
} // namespace mbgl
