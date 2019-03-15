#include <mbgl/programs/raster_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<RasterProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(RasterLayoutVertex) == 8, "expected RasterLayoutVertex size");

} // namespace mbgl
