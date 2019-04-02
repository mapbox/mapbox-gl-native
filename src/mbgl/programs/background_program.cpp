#include <mbgl/programs/background_program.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(BackgroundLayoutVertex) == 4, "expected BackgroundLayoutVertex size");

BackgroundPatternProgram::LayoutUniformValues
BackgroundPatternProgram::layoutUniformValues(mat4 matrix,
                                              float opacity,
                                              Size atlasSize,
                                              const ImagePosition& a,
                                              const ImagePosition& b,
                                              const CrossfadeParameters& fading,
                                              const UnwrappedTileID& tileID,
                                              const TransformState& state) {
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;

    return {
        uniforms::matrix::Value( matrix ),
        uniforms::opacity::Value( opacity ),
        uniforms::texsize::Value( atlasSize ),
        uniforms::pattern_tl_a::Value( a.tl() ),
        uniforms::pattern_br_a::Value( a.br() ),
        uniforms::pattern_tl_b::Value( b.tl() ),
        uniforms::pattern_br_b::Value( b.br() ),
        uniforms::pattern_size_a::Value( a.displaySize() ),
        uniforms::pattern_size_b::Value( b.displaySize() ),
        uniforms::scale_a::Value( fading.fromScale ),
        uniforms::scale_b::Value( fading.toScale ),
        uniforms::mix::Value( fading.t ),
        uniforms::pixel_coord_upper::Value( std::array<float, 2> {{ float(pixelX >> 16), float(pixelY >> 16) }}),
        uniforms::pixel_coord_lower::Value( std::array<float, 2> {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF)}}),
        uniforms::tile_units_to_pixels::Value( 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom()) ),
    };
}

} // namespace mbgl
