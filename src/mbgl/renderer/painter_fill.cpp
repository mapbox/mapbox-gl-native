#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFill(PaintParameters& parameters,
                         FillBucket& bucket,
                         const FillLayer& layer,
                         const RenderTile& tile) {
    const FillPaintProperties& properties = layer.impl->paint;
    mat4 vertexMatrix = tile.translatedMatrix(properties.fillTranslate,
                                              properties.fillTranslateAnchor,
                                              state);

    Color fillColor = properties.fillColor;
    float opacity = properties.fillOpacity;

    const bool isOutlineColorDefined = !properties.fillOutlineColor.isUndefined();
    Color strokeColor = isOutlineColorDefined? properties.fillOutlineColor : fillColor;

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

    auto& outlineShader = parameters.shaders.outline;
    auto& patternShader = parameters.shaders.pattern;
    auto& outlinePatternShader = parameters.shaders.outlinePattern;
    auto& plainShader = parameters.shaders.plain;

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // befrom, we have to draw the outline first (!)
    if (outline && pass == RenderPass::Translucent) {
        config.program = outlineShader.getID();
        outlineShader.u_matrix = vertexMatrix;

        outlineShader.u_outline_color = strokeColor;
        outlineShader.u_opacity = opacity;

        // Draw the entire line
        outlineShader.u_world = worldSize;
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
        bucket.drawVertices(outlineShader, store, isOverdraw());
    }

    if (pattern) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(properties.fillPattern.value.from, true);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(properties.fillPattern.value.to, true);

        // Image fill.
        if (pass == RenderPass::Translucent && imagePosA && imagePosB) {
            config.program = patternShader.getID();
            patternShader.u_matrix = vertexMatrix;
            patternShader.u_pattern_tl_a = imagePosA->tl;
            patternShader.u_pattern_br_a = imagePosA->br;
            patternShader.u_pattern_tl_b = imagePosB->tl;
            patternShader.u_pattern_br_b = imagePosB->br;
            patternShader.u_opacity = properties.fillOpacity;
            patternShader.u_image = 0;
            patternShader.u_mix = properties.fillPattern.value.t;
            patternShader.u_pattern_size_a = imagePosA->size;
            patternShader.u_pattern_size_b = imagePosB->size;
            patternShader.u_scale_a = properties.fillPattern.value.fromScale;
            patternShader.u_scale_b = properties.fillPattern.value.toScale;
            patternShader.u_tile_units_to_pixels = 1.0f / tile.id.pixelsToTileUnits(1.0f, state.getIntegerZoom());

            GLint tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tile.id.canonical.z);
            GLint pixelX = tileSizeAtNearestZoom * (tile.id.canonical.x + tile.id.wrap * state.zoomScale(tile.id.canonical.z));
            GLint pixelY = tileSizeAtNearestZoom * tile.id.canonical.y;
            patternShader.u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
            patternShader.u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};

            spriteAtlas->bind(true, store, config, 0);

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(0);
            bucket.drawElements(patternShader, store, isOverdraw());

            if (properties.fillAntialias && !isOutlineColorDefined) {
                config.program = outlinePatternShader.getID();
                outlinePatternShader.u_matrix = vertexMatrix;

                outlinePatternShader.u_pattern_tl_a = imagePosA->tl;
                outlinePatternShader.u_pattern_br_a = imagePosA->br;
                outlinePatternShader.u_pattern_tl_b = imagePosB->tl;
                outlinePatternShader.u_pattern_br_b = imagePosB->br;
                outlinePatternShader.u_opacity = properties.fillOpacity;
                outlinePatternShader.u_image = 0;
                outlinePatternShader.u_mix = properties.fillPattern.value.t;
                outlinePatternShader.u_pattern_size_a = imagePosA->size;
                outlinePatternShader.u_pattern_size_b = imagePosB->size;
                outlinePatternShader.u_scale_a = properties.fillPattern.value.fromScale;
                outlinePatternShader.u_scale_b = properties.fillPattern.value.toScale;
                outlinePatternShader.u_tile_units_to_pixels = 1.0f / tile.id.pixelsToTileUnits(1.0f, state.getIntegerZoom());
                outlinePatternShader.u_pixel_coord_upper = {{ float(pixelX >> 16), float(pixelY >> 16) }};
                outlinePatternShader.u_pixel_coord_lower = {{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }};

                // Draw the entire line
                outlinePatternShader.u_world = worldSize;

                spriteAtlas->bind(true, store, config, 0);

                setDepthSublayer(2);
                bucket.drawVertices(outlinePatternShader, store, isOverdraw());
            }
        }
    } else {
        // No image fill.
        if ((fillColor.a >= 1.0f && opacity >= 1.0f) == (pass == RenderPass::Opaque)) {
            // Only draw the fill when it's either opaque and we're drawing opaque
            // fragments or when it's translucent and we're drawing translucent
            // fragments
            // Draw filling rectangle.
            config.program = plainShader.getID();
            plainShader.u_matrix = vertexMatrix;
            plainShader.u_color = fillColor;
            plainShader.u_opacity = opacity;

            // Draw the actual triangles into the color & stencil buffer.
            setDepthSublayer(1);
            bucket.drawElements(plainShader, store, isOverdraw());
        }
    }

    // Because we're drawing top-to-bottom, and we update the stencil mask
    // below, we have to draw the outline first (!)
    if (fringeline && pass == RenderPass::Translucent) {
        config.program = outlineShader.getID();
        outlineShader.u_matrix = vertexMatrix;

        outlineShader.u_outline_color = fillColor;
        outlineShader.u_opacity = opacity;

        // Draw the entire line
        outlineShader.u_world = worldSize;

        setDepthSublayer(2);
        bucket.drawVertices(outlineShader, store, isOverdraw());
    }
}

} // namespace mbgl
