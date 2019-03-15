#include <mbgl/programs/extrusion_texture_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<ExtrusionTextureProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(ExtrusionTextureLayoutVertex) == 4, "expected ExtrusionTextureLayoutVertex size");

} // namespace mbgl
