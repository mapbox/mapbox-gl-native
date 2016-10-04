#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/gl/gl.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/tile_cover.hpp>

namespace mbgl {

using namespace style;

void Painter::renderBackground(PaintParameters& parameters, const BackgroundLayer& layer) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.
    const BackgroundPaintProperties& properties = layer.impl->paint;

    bool isPatterned = !properties.backgroundPattern.value.to.empty();// && false;
    optional<SpriteAtlasPosition> imagePosA;
    optional<SpriteAtlasPosition> imagePosB;

    auto& patternShader = parameters.shaders.fillPattern;
    auto& plainShader = parameters.shaders.fill;
    auto& arrayBackgroundPattern = parameters.shaders.backgroundPatternArray;
    auto& arrayBackground = parameters.shaders.backgroundArray;

    if (isPatterned) {
        imagePosA = spriteAtlas->getPosition(properties.backgroundPattern.value.from,
                                             SpritePatternMode::Repeating);
        imagePosB = spriteAtlas->getPosition(properties.backgroundPattern.value.to,
                                             SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB)
            return;

        context.program = patternShader.getID();
        patternShader.u_matrix = identityMatrix;
        patternShader.u_pattern_tl_a = imagePosA->tl;
        patternShader.u_pattern_br_a = imagePosA->br;
        patternShader.u_pattern_tl_b = imagePosB->tl;
        patternShader.u_pattern_br_b = imagePosB->br;
        patternShader.u_mix = properties.backgroundPattern.value.t;
        patternShader.u_opacity = properties.backgroundOpacity;

        spriteAtlas->bind(true, context, 0);
        arrayBackgroundPattern.bind(patternShader, tileTriangleVertexBuffer, BUFFER_OFFSET(0), context);

    } else {
        context.program = plainShader.getID();
        plainShader.u_color = properties.backgroundColor;
        plainShader.u_opacity = properties.backgroundOpacity;

        arrayBackground.bind(plainShader, tileTriangleVertexBuffer, BUFFER_OFFSET(0), context);
    }

    context.stencilTest = false;
    context.depthFunc = gl::DepthTestFunction::LessEqual;
    context.depthTest = true;
    context.depthMask = false;
    setDepthSublayer(0);

    for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
        mat4 vertexMatrix;
        state.matrixFor(vertexMatrix, tileID);
        matrix::multiply(vertexMatrix, projMatrix, vertexMatrix);

        if (isPatterned) {
            patternShader.u_matrix = vertexMatrix;
            patternShader.u_pattern_size_a = imagePosA->size;
            patternShader.u_pattern_size_b = imagePosB->size;
            patternShader.u_scale_a = properties.backgroundPattern.value.fromScale;
            patternShader.u_scale_b = properties.backgroundPattern.value.toScale;
            patternShader.u_tile_units_to_pixels = 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom());

            GLint tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
            GLint pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
            GLint pixelY = tileSizeAtNearestZoom * tileID.canonical.y;
            patternShader.u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
            patternShader.u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};
        } else {
            plainShader.u_matrix = vertexMatrix;
        }

        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(tileTriangleVertexBuffer.vertexCount)));
    }
}

} // namespace mbgl
