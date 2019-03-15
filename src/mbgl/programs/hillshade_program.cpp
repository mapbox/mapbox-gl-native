#include <mbgl/programs/hillshade_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<HillshadeProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(HillshadeLayoutVertex) == 8, "expected HillshadeLayoutVertex size");

} // namespace mbgl
