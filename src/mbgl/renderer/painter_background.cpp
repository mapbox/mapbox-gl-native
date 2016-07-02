#include <mbgl/renderer/painter.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/tile_cover.hpp>

namespace mbgl {

using namespace style;

void Painter::renderBackground(const BackgroundLayer& layer) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.
    const BackgroundPaintProperties& properties = layer.impl->paint;

    bool isPatterned = !properties.backgroundPattern.value.to.empty();// && false;
    optional<SpriteAtlasPosition> imagePosA;
    optional<SpriteAtlasPosition> imagePosB;

    const bool overdraw = isOverdraw();
    const auto& shaderPattern = overdraw ? patternOverdrawShader : patternShader;
    const auto& shaderPlain = overdraw ? plainOverdrawShader : plainShader;
    auto& arrayBackgroundPattern = overdraw ? backgroundPatternOverdrawArray : backgroundPatternArray;
    auto& arrayBackground = overdraw ? backgroundOverdrawArray : backgroundArray;

    if (isPatterned) {
        imagePosA = spriteAtlas->getPosition(properties.backgroundPattern.value.from, true);
        imagePosB = spriteAtlas->getPosition(properties.backgroundPattern.value.to, true);

        if (!imagePosA || !imagePosB)
            return;

        config.program = shaderPattern->getID();
        shaderPattern->u_matrix = identityMatrix;
        shaderPattern->u_pattern_tl_a = imagePosA->tl;
        shaderPattern->u_pattern_br_a = imagePosA->br;
        shaderPattern->u_pattern_tl_b = imagePosB->tl;
        shaderPattern->u_pattern_br_b = imagePosB->br;
        shaderPattern->u_mix = properties.backgroundPattern.value.t;
        shaderPattern->u_opacity = properties.backgroundOpacity;

        spriteAtlas->bind(true, store);
        arrayBackgroundPattern.bind(*shaderPattern, tileStencilBuffer, BUFFER_OFFSET(0), store);

    } else {
        config.program = shaderPlain->getID();
        shaderPlain->u_color = properties.backgroundColor;
        shaderPlain->u_opacity = properties.backgroundOpacity;

        arrayBackground.bind(*shaderPlain, tileStencilBuffer, BUFFER_OFFSET(0), store);
    }

    config.stencilTest = GL_FALSE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
    config.depthMask = GL_FALSE;
    setDepthSublayer(0);

    for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
        mat4 vertexMatrix;
        state.matrixFor(vertexMatrix, tileID);
        matrix::multiply(vertexMatrix, projMatrix, vertexMatrix);

        if (isPatterned) {
            shaderPattern->u_matrix = vertexMatrix;
            shaderPattern->u_pattern_size_a = imagePosA->size;
            shaderPattern->u_pattern_size_b = imagePosB->size;
            shaderPattern->u_scale_a = properties.backgroundPattern.value.fromScale;
            shaderPattern->u_scale_b = properties.backgroundPattern.value.toScale;
            shaderPattern->u_tile_units_to_pixels = 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom());

            GLint tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
            GLint pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
            GLint pixelY = tileSizeAtNearestZoom * tileID.canonical.y;
            shaderPattern->u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
            shaderPattern->u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};
        } else {
            shaderPlain->u_matrix = vertexMatrix;
        }

        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)tileStencilBuffer.index()));
    }
}

} // namespace mbgl
