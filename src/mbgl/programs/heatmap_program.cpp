#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<HeatmapProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(HeatmapLayoutVertex) == 4, "expected HeatmapLayoutVertex size");

} // namespace mbgl
