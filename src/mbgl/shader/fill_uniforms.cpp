#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

FillPatternUniforms::Values
FillPatternUniforms::values(mat4 matrix,
                            float opacity,
                            Size framebufferSize,
                            const SpriteAtlasPosition& a,
                            const SpriteAtlasPosition& b,
                            const Faded<std::string>& fading,
                            const UnwrappedTileID& tileID,
                            const TransformState& state)
{
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;

    return FillPatternUniforms::Values {
        matrix,
        opacity,
        framebufferSize,
        a.tl,
        a.br,
        b.tl,
        b.br,
        a.size,
        b.size,
        fading.fromScale,
        fading.toScale,
        fading.t,
        0,
        std::array<float, 2> {{ float(pixelX >> 16), float(pixelY >> 16) }},
        std::array<float, 2> {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }},
        1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom())
    };
}

} // namespace mbgl
