#include <mbgl/programs/fill_program.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

static_assert(sizeof(FillLayoutVertex) == 4, "expected FillLayoutVertex size");

FillPatternUniforms::Values
FillPatternUniforms::values(mat4 matrix,
                            Size framebufferSize,
                            Size atlasSize,
                            const CrossfadeParameters& crossfade,
                            const UnwrappedTileID& tileID,
                            const TransformState& state,
                            const float pixelRatio)
{
    const auto tileRatio = 1 / tileID.pixelsToTileUnits(1, state.getIntegerZoom());
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;

    return FillPatternUniforms::Values {
        uniforms::u_matrix::Value( matrix ),
        uniforms::u_world::Value( framebufferSize ),
        uniforms::u_texsize::Value( atlasSize ),
        uniforms::u_scale::Value({ {pixelRatio, tileRatio, crossfade.fromScale, crossfade.toScale} } ),
        uniforms::u_fade::Value( crossfade.t ),
        uniforms::u_image::Value( 0 ),
        uniforms::u_pixel_coord_upper::Value( std::array<float, 2> {{ float(pixelX >> 16), float(pixelY >> 16) }}),
        uniforms::u_pixel_coord_lower::Value( std::array<float, 2> {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }} )
    };
}

} // namespace mbgl
