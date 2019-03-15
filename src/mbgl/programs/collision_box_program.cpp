#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<CollisionBoxProgram>> gfx::Context::createProgram(const ProgramParameters&);
template std::unique_ptr<gfx::Program<CollisionCircleProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(CollisionBoxProgram::LayoutVertex) == 24, "expected CollisionBoxVertex size");

} // namespace mbgl
