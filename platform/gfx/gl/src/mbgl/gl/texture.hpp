#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/literal.hpp>
#include <mbgl/util/ignore.hpp>

#include <vector>
#include <string>

namespace mbgl {
namespace gl {

class Context;

void bindTexture(gl::Context&, uint8_t unit, const gfx::TextureBinding&);

template <class>
class TextureStates;

template <class... Ts>
class TextureStates<TypeList<Ts...>> {
private:
    using State =
        IndexedTuple<TypeList<Ts...>, TypeList<ExpandToType<Ts, gl::UniformState<uint8_t>>...>>;

    State state;

public:
    void queryLocations(const ProgramID& id) {
        state = State{ gl::uniformLocation(id,
            concat_literals<&string_literal<'u', '_'>::value, &Ts::name>::value())... };
    }

    NamedUniformLocations getNamedLocations() const {
        return NamedUniformLocations{ { concat_literals<&string_literal<'u', '_'>::value, &Ts::name>::value(),
                                        state.template get<Ts>().location }... };
    }

    void bind(gl::Context& context, const gfx::TextureBindings<TypeList<Ts...>>& bindings) {
        util::ignore(
            { (state.template get<Ts>() = TypeIndex<Ts, Ts...>::value,
               gl::bindTexture(context, TypeIndex<Ts, Ts...>::value, bindings.template get<Ts>()),
               0)... });
    }
};

} // namespace gl
} // namespace mbgl
