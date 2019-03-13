#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/ignore.hpp>

#include <vector>
#include <string>

namespace mbgl {
namespace gl {

class Context;

void bindTexture(gl::Context&, uint8_t unit, const gfx::TextureBinding&);

template <class>
class Textures;

template <class... Ts>
class Textures<TypeList<Ts...>> {
    using State =
        IndexedTuple<TypeList<Ts...>, TypeList<ExpandToType<Ts, gl::UniformState<uint8_t>>...>>;
    using NamedLocations = std::vector<std::pair<const std::string, gl::UniformLocation>>;

public:
    void queryLocations(const ProgramID& id) {
        state = State{ gl::uniformLocation(id, Ts::name())... };
    }

    template <class BinaryProgram>
    void loadNamedLocations(const BinaryProgram& program) {
        state = State{ program.textureLocation(Ts::name())... };
    }

    NamedLocations getNamedLocations() const {
        return NamedLocations{ { Ts::name(), state.template get<Ts>().location }... };
    }

    void bind(gl::Context& context, const gfx::TextureBindings<TypeList<Ts...>>& bindings) {
        util::ignore(
            { (state.template get<Ts>() = TypeIndex<Ts, Ts...>::value,
               gl::bindTexture(context, TypeIndex<Ts, Ts...>::value, bindings.template get<Ts>()),
               0)... });
    }

private:
    State state;
};

} // namespace gl
} // namespace mbgl
