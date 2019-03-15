#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<HeatmapTextureProgram>> gfx::Context::createProgram(const ProgramParameters&);

static_assert(sizeof(HeatmapTextureLayoutVertex) == 4, "expected HeatmapTextureLayoutVertex size");

} // namespace mbgl
