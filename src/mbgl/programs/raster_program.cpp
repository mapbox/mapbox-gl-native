#include <mbgl/programs/raster_program.hpp>

namespace mbgl {

static_assert(sizeof(RasterProgram::Vertex) == 8, "expected RasterVertex size");

} // namespace mbgl
