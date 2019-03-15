#pragma once

#include <mbgl/gfx/context.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

namespace gfx {

template <typename Name>
std::unique_ptr<Program<Name>> Context::createProgram(const ProgramParameters& programParameters) {
    if (backend == ContextType::OpenGL) {
        return createProgram<gl::Context, Name>(programParameters);
    }
    assert(false);
    return nullptr;
}

} // namespace gfx
} // namespace mbgl
