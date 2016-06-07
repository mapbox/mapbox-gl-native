#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/symbol_renderable.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

using namespace style;

namespace {

void drawGlyphs(SymbolRenderable& renderable, SDFShader& shader, gl::ObjectStore& store) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    auto& text = renderable.text;
    for (auto& group : text.groups) {
        assert(group);
        group->array[0].bind(shader, text.vertices, text.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * text.vertices.itemSize;
        elements_index += group->elements_length * text.triangles.itemSize;
    }
}

void drawIcons(SymbolRenderable& renderable, SDFShader& shader, gl::ObjectStore& store) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    auto& icon = renderable.icon;
    for (auto& group : icon.groups) {
        assert(group);
        group->array[0].bind(shader, icon.vertices, icon.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void drawIcons(SymbolRenderable& renderable, IconShader& shader, gl::ObjectStore& store) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    GLbyte* elements_index = BUFFER_OFFSET_0;
    auto& icon = renderable.icon;
    for (auto& group : icon.groups) {
        assert(group);
        group->array[1].bind(shader, icon.vertices, icon.triangles, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * icon.vertices.itemSize;
        elements_index += group->elements_length * icon.triangles.itemSize;
    }
}

void drawCollisionBoxes(SymbolRenderable& renderable,
                        CollisionBoxShader& shader,
                        gl::ObjectStore& store) {
    GLbyte* vertex_index = BUFFER_OFFSET_0;
    auto& collisionBox = renderable.collisionBox;
    for (auto& group : collisionBox.groups) {
        group->array[0].bind(shader, collisionBox.vertices, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, group->vertex_length));
    }
}

} // namespace

void Painter::renderSDF(SymbolRenderable &renderable,
                        const UnwrappedTileID &tileID,
                        const mat4 &matrix,
                        float sdfFontSize,
                        std::array<float, 2> texsize,
                        SDFShader& sdfShader,
                        void (*drawSDF)(SymbolRenderable&, SDFShader&, gl::ObjectStore&),

                        // Layout
                        RotationAlignmentType rotationAlignment,
                        float layoutSize,

                        // Paint
                        float opacity,
                        Color color,
                        Color haloColor,
                        float haloWidth,
                        float haloBlur,
                        std::array<float, 2> translate,
                        TranslateAnchorType translateAnchor,
                        float paintSize)
{
    mat4 vtxMatrix = translatedMatrix(matrix, translate, tileID, translateAnchor);

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = paintSize;
    float fontScale = fontSize / sdfFontSize;

    float scale = fontScale;
    std::array<float, 2> exScale = extrudeScale;
    bool alignedWithMap = rotationAlignment == RotationAlignmentType::Map;
    float gammaScale = 1.0f;

    if (alignedWithMap) {
        scale *= tileID.pixelsToTileUnits(1, state.getZoom());
        exScale.fill(scale);
        gammaScale /= std::cos(state.getPitch());
    } else {
        exScale = {{ exScale[0] * scale, exScale[1] * scale }};
    }

    config.program = sdfShader.getID();
    sdfShader.u_matrix = vtxMatrix;
    sdfShader.u_extrude_scale = exScale;
    sdfShader.u_texsize = texsize;
    sdfShader.u_skewed = alignedWithMap;
    sdfShader.u_texture = 0;

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(fontSize / layoutSize) / std::log(2);

    sdfShader.u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level

    config.activeTexture = GL_TEXTURE1;
    frameHistory.bind(store);
    sdfShader.u_fadetexture = 1;

    // The default gamma value has to be adjust for the current pixelratio so that we're not
    // drawing blurry font on retina screens.
    const float gamma = 0.105 * sdfFontSize / fontSize / frame.pixelRatio;

    const float sdfPx = 8.0f;
    const float blurOffset = 1.19f;
    const float haloOffset = 6.0f;

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (haloColor[3] > 0.0f && haloWidth > 0.0f) {
        sdfShader.u_gamma = (haloBlur * blurOffset / fontScale / sdfPx + gamma) * gammaScale;
        sdfShader.u_color = haloColor;
        sdfShader.u_opacity = opacity;
        sdfShader.u_buffer = (haloOffset - haloWidth / fontScale) / sdfPx;

        setDepthSublayer(0);
        drawSDF(renderable, sdfShader, store);
    }

    // Then, we draw the text/icon over the halo
    if (color[3] > 0.0f) {
        sdfShader.u_gamma = gamma * gammaScale;
        sdfShader.u_color = color;
        sdfShader.u_opacity = opacity;
        sdfShader.u_buffer = (256.0f - 64.0f) / 256.0f;

        setDepthSublayer(1);
        drawSDF(renderable, sdfShader, store);
    }
}

void Painter::renderSymbol(SymbolBucket& bucket,
                           const SymbolLayer& layer,
                           const UnwrappedTileID& tileID,
                           const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto& paint = layer.impl->paint;
    const auto& layout = bucket.layout;

    config.depthMask = GL_FALSE;

    // TODO remove the `true ||` when #1673 is implemented
    const bool drawAcrossEdges = (frame.mapMode == MapMode::Continuous) && (true || !(layout.textAllowOverlap || layout.iconAllowOverlap ||
          layout.textIgnorePlacement || layout.iconIgnorePlacement));

    // Disable the stencil test so that labels aren't clipped to tile boundaries.
    //
    // Layers with features that may be drawn overlapping aren't clipped. These
    // layers are sorted in the y direction, and to draw the correct ordering near
    // tile edges the icons are included in both tiles and clipped when drawing.
    if (drawAcrossEdges) {
        config.stencilTest = GL_FALSE;
    } else {
        config.stencilOp.reset();
        config.stencilTest = GL_TRUE;
    }

    if (bucket.hasIconData()) {
        if (layout.iconRotationAlignment == RotationAlignmentType::Map) {
            config.depthFunc.reset();
            config.depthTest = GL_TRUE;
        } else {
            config.depthTest = GL_FALSE;
        }

        bool sdf = bucket.sdfIcons;

        const float angleOffset =
            layout.iconRotationAlignment == RotationAlignmentType::Map
                ? state.getAngle()
                : 0;

        const float fontSize = layer.impl->iconSize;
        const float fontScale = fontSize / 1.0f;

        SpriteAtlas* activeSpriteAtlas = layer.impl->spriteAtlas;
        const bool iconScaled = fontScale != 1 || frame.pixelRatio != activeSpriteAtlas->getPixelRatio() || bucket.iconsNeedLinear;
        const bool iconTransformed = layout.iconRotationAlignment == RotationAlignmentType::Map || angleOffset != 0 || state.getPitch() != 0;
        config.activeTexture = GL_TEXTURE0;
        activeSpriteAtlas->bind(sdf || state.isChanging() || iconScaled || iconTransformed, store);

        if (sdf) {
            renderSDF(bucket.getRenderable(),
                      tileID,
                      matrix,
                      1.0f,
                      {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }},
                      *sdfIconShader,
                      &drawIcons,
                      layout.iconRotationAlignment,
                      layout.iconSize,
                      paint.iconOpacity,
                      paint.iconColor,
                      paint.iconHaloColor,
                      paint.iconHaloWidth,
                      paint.iconHaloBlur,
                      paint.iconTranslate,
                      paint.iconTranslateAnchor,
                      layer.impl->iconSize);
        } else {
            mat4 vtxMatrix =
                translatedMatrix(matrix, paint.iconTranslate, tileID, paint.iconTranslateAnchor);

            float scale = fontScale;
            std::array<float, 2> exScale = extrudeScale;
            const bool alignedWithMap = layout.iconRotationAlignment == RotationAlignmentType::Map;
            if (alignedWithMap) {
                scale *= tileID.pixelsToTileUnits(1, state.getZoom());
                exScale.fill(scale);
            } else {
                exScale = {{ exScale[0] * scale, exScale[1] * scale }};
            }

            config.program = iconShader->getID();
            iconShader->u_matrix = vtxMatrix;
            iconShader->u_extrude_scale = exScale;
            iconShader->u_texsize = {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }};
            iconShader->u_skewed = alignedWithMap;
            iconShader->u_texture = 0;

            // adjust min/max zooms for variable font sies
            float zoomAdjust = std::log(fontSize / layout.iconSize) / std::log(2);
            iconShader->u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level
            iconShader->u_opacity = paint.iconOpacity;

            config.activeTexture = GL_TEXTURE1;
            frameHistory.bind(store);
            iconShader->u_fadetexture = 1;

            setDepthSublayer(0);
            drawIcons(bucket.getRenderable(), *iconShader, store);
        }
    }

    if (bucket.hasTextData()) {
        if (layout.textRotationAlignment == RotationAlignmentType::Map) {
            config.depthFunc.reset();
            config.depthTest = GL_TRUE;
        } else {
            config.depthTest = GL_FALSE;
        }

        config.activeTexture = GL_TEXTURE0;
        glyphAtlas->bind(store);

        renderSDF(bucket.getRenderable(),
                  tileID,
                  matrix,
                  24.0f,
                  {{ float(glyphAtlas->width) / 4, float(glyphAtlas->height) / 4 }},
                  *sdfGlyphShader,
                  &drawGlyphs,
                  layout.textRotationAlignment,
                  layout.textSize,
                  paint.textOpacity,
                  paint.textColor,
                  paint.textHaloColor,
                  paint.textHaloWidth,
                  paint.textHaloBlur,
                  paint.textTranslate,
                  paint.textTranslateAnchor,
                  layer.impl->textSize);
    }

    if (bucket.hasCollisionBoxData()) {
        config.stencilOp.reset();
        config.stencilTest = GL_TRUE;

        config.program = collisionBoxShader->getID();
        collisionBoxShader->u_matrix = matrix;
        // TODO: This was the overscaled z instead of the canonical z.
        collisionBoxShader->u_scale = std::pow(2, state.getZoom() - tileID.canonical.z);
        collisionBoxShader->u_zoom = state.getZoom() * 10;
        collisionBoxShader->u_maxzoom = (tileID.canonical.z + 1) * 10;
        config.lineWidth = 1.0f;

        setDepthSublayer(0);
        drawCollisionBoxes(bucket.getRenderable(), *collisionBoxShader, store);
    }

    config.activeTexture = GL_TEXTURE0;
}

} // namespace mbgl
