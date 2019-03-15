#include <mbgl/programs/debug_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<DebugProgram>> gfx::Context::createProgram(const ProgramParameters&);

} // namespace mbgl
