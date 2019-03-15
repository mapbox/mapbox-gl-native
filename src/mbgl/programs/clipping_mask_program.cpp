#include <mbgl/programs/clipping_mask_program.hpp>
#include <mbgl/gfx/context_impl.hpp>

namespace mbgl {

template std::unique_ptr<gfx::Program<ClippingMaskProgram>> gfx::Context::createProgram(const ProgramParameters&);

} // namespace mbgl
