#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFill(FillBucket& bucket,
                         const FillLayer& layer,
                         const UnwrappedTileID& tileID,
                         const mat4& matrix) {
    const FillPaintProperties& properties = layer.impl->paint;
    mat4 vertexMatrix =
        translatedMatrix(matrix, properties.fillTranslate, tileID, properties.fillTranslateAnchor);

    Color fillColor = properties.fillColor;
    float opacity = properties.fillOpacity;

    Color strokeColor = properties.fillOutlineColor;
    bool isOutlineColorDefined = strokeColor.a >= 0;
    if (!isOutlineColorDefined) {
        strokeColor = fillColor;
    }

    auto worldSize = util::convert<GLfloat>(frame.framebufferSize);

    bool pattern = !properties.fillPattern.value.from.empty();
    bool outline = properties.fillAntialias && !pattern && isOutlineColorDefined;
    bool fringeline = properties.fillAntialias && !pattern && !isOutlineColorDefined;

    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
    config.depthMask = GL_TRUE;
    config.lineWidth = 2.0f; // This is always fixed and does not depend on the pixelRatio!

    const bool overdraw = isOverdraw();
    const auto& shaderOutline = overdraw ? outlineOverdrawShader : outlineShader;
    const auto& shaderPattern = overdraw ? patternOverdrawShader : patternShader;
    const auto& shaderOutlinePattern = overdraw ? outlinePatternOverdrawShader : outlinePatternShader;
    const auto& shaderPlain = overdraw ? plainOverdrawShader : plainShader;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // befrom, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {

        config.program = shaderOutline->getID();
        shaderOutline->u_matrix = vertexMatrix;

        shaderOutline->u_outline_color = strokeColor;
        shaderOutline->u_opacity = opacity;

        // Draw the entire line
        shaderOutline->u_world = worldSize;
        if (isOutlineColorDefined) {
            // If we defined a different color for the fill outline, we are
            // going to ignore the bits in 0x07 and just care about the global
            // clipping mask.
            setDepthSublayer(2); // OK
        } else {
            // Otherwise, we only want to drawFill the antialiased parts that are
            // *outside* the current shape. This is important in case the fill
            // or stroke color is translucent. If we wouldn't clip to outside
            // the current shape, some pixels from the outline stroke overlapped
            // the (non-antialiased) fill.
            setDepthSublayer(0); // OK
        }
        bucket.drawVertices(*shaderOutline, store, overdraw);
    }

    if (pattern) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(properties.fillPattern.value.from, true);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(properties.fillPattern.value.to, true);

        // Image fill.
        if (pass == RenderPass::Translucent && imagePosA && imagePosB) {
            config.program = shaderPattern->getID();
            shaderPattern->u_matrix = vertexMatrix;
            shaderPattern->u_pattern_tl_a = imagePosA->tl;
            shaderPattern->u_pattern_br_a = imagePosA->br;
            shaderPattern->u_pattern_tl_b = imagePosB->tl;
            shaderPattern->u_pattern_br_b = imagePosB->br;
            shaderPattern->u_opacity = properties.fillOpacity;
            shaderPattern->u_image = 0;
            shaderPattern->u_mix = properties.fillPattern.value.t;
            shaderPattern->u_pattern_size_a = imagePosA->size;
            shaderPattern->u_pattern_size_b = imagePosB->size;
            shaderPattern->u_scale_a = properties.fillPattern.value.fromScale;
            shaderPattern->u_scale_b = properties.fillPattern.value.toScale;
            shaderPattern->u_tile_units_to_pixels = 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom());

            GLint tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
            GLint pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
            GLint pixelY = tileSizeAtNearestZoom * tileID.canonical.y;
            shaderPattern->u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
            shaderPattern->u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};

            config.activeTexture = GL_TEXTURE0;
            spriteAtlas->bind(true, store);

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(0);
            bucket.drawElements(*shaderPattern, store, overdraw);

            if (properties.fillAntialias && !isOutlineColorDefined) {
                config.program = shaderOutlinePattern->getID();
                shaderOutlinePattern->u_matrix = vertexMatrix;

                shaderOutlinePattern->u_pattern_tl_a = imagePosA->tl;
                shaderOutlinePattern->u_pattern_br_a = imagePosA->br;
                shaderOutlinePattern->u_pattern_tl_b = imagePosB->tl;
                shaderOutlinePattern->u_pattern_br_b = imagePosB->br;
                shaderOutlinePattern->u_opacity = properties.fillOpacity;
                shaderOutlinePattern->u_image = 0;
                shaderOutlinePattern->u_mix = properties.fillPattern.value.t;
                shaderOutlinePattern->u_pattern_size_a = imagePosA->size;
                shaderOutlinePattern->u_pattern_size_b = imagePosB->size;
                shaderOutlinePattern->u_scale_a = properties.fillPattern.value.fromScale;
                shaderOutlinePattern->u_scale_b = properties.fillPattern.value.toScale;
                shaderOutlinePattern->u_tile_units_to_pixels = 1.0f / tileID.pixelsToTileUnits(1.0f, state.getIntegerZoom());
                shaderOutlinePattern->u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
                shaderOutlinePattern->u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};

                // Draw the entire line
                shaderOutlinePattern->u_world = worldSize;

                config.activeTexture = GL_TEXTURE0;
                spriteAtlas->bind(true, store);

                setDepthSublayer(2);
                bucket.drawVertices(*shaderOutlinePattern, store, overdraw);
            }
        }
    } else {
        // No image fill.
        if ((fillColor.a >= 1.0f && opacity >= 1.0f) == (pass == RenderPass::Opaque)) {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            config.program = shaderPlain->getID();
            shaderPlain->u_matrix = vertexMatrix;
            shaderPlain->u_color = fillColor;
            shaderPlain->u_opacity = opacity;

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(1);
            bucket.drawElements(*shaderPlain, store, overdraw);
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        config.program = shaderOutline->getID();
        shaderOutline->u_matrix = vertexMatrix;

        shaderOutline->u_outline_color = fillColor;
        shaderOutline->u_opacity = opacity;

        // Draw the entire line
        shaderOutline->u_world = worldSize;

        setDepthSublayer(2);
        bucket.drawVertices(*shaderOutline, store, overdraw);
    }
}

} // namespace mbgl
