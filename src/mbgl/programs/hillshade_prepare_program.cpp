#include <mbgl/programs/hillshade_prepare_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<HillshadePrepareProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(HillshadePrepareLayoutVertex) == 8, "expected HillshadeLayoutVertex size");

} // namespace mbgl
