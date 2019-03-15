#include <mbgl/programs/circle_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<CircleProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(CircleLayoutVertex) == 4, "expected CircleLayoutVertex size");

} // namespace mbgl
