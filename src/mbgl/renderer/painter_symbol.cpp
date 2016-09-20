#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

using namespace style;

void Painter::renderSDF(SymbolBucket& bucket,
                        const RenderTile& tile,
                        float sdfFontSize,
                        std::array<float, 2> texsize,
                        SDFShader& sdfShader,
                        void (SymbolBucket::*drawSDF)(SDFShader&, gl::ObjectStore&, PaintMode),

                        // Layout
                        AlignmentType rotationAlignment,
                        AlignmentType pitchAlignment,
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
    mat4 vtxMatrix = tile.translatedMatrix(translate, translateAnchor, state);

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = paintSize;
    float fontScale = fontSize / sdfFontSize;

    bool rotateWithMap = rotationAlignment == AlignmentType::Map;
    bool pitchWithMap = pitchAlignment == AlignmentType::Map;

    std::array<float, 2> extrudeScale;
    float gammaScale;

    if (pitchWithMap) {
        gammaScale = 1.0 / std::cos(state.getPitch());
        extrudeScale.fill(tile.id.pixelsToTileUnits(1, state.getZoom()) * fontScale);
    } else {
        gammaScale = 1.0;
        extrudeScale = {{
            pixelsToGLUnits[0] * fontScale * state.getAltitude(),
            pixelsToGLUnits[1] * fontScale * state.getAltitude()
        }};
    }

    config.program = sdfShader.getID();
    sdfShader.u_matrix = vtxMatrix;
    sdfShader.u_extrude_scale = extrudeScale;
    sdfShader.u_texsize = texsize;
    sdfShader.u_rotate_with_map = rotateWithMap;
    sdfShader.u_pitch_with_map = pitchWithMap;
    sdfShader.u_texture = 0;
    sdfShader.u_pitch = state.getPitch() * util::DEG2RAD;
    sdfShader.u_bearing = -1.0f * state.getAngle();
    sdfShader.u_aspect_ratio = (state.getWidth() * 1.0f) / (state.getHeight() * 1.0f);

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(fontSize / layoutSize) / std::log(2);

    sdfShader.u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level

    frameHistory.bind(store, config, 1);
    sdfShader.u_fadetexture = 1;

    // The default gamma value has to be adjust for the current pixelratio so that we're not
    // drawing blurry font on retina screens.
    const float gamma = 0.105 * sdfFontSize / fontSize / frame.pixelRatio;

    const float sdfPx = 8.0f;
    const float blurOffset = 1.19f;
    const float haloOffset = 6.0f;

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (haloColor.a > 0.0f && haloWidth > 0.0f) {
        sdfShader.u_gamma = (haloBlur * blurOffset / fontScale / sdfPx + gamma) * gammaScale;
        sdfShader.u_color = haloColor;
        sdfShader.u_opacity = opacity;
        sdfShader.u_buffer = (haloOffset - haloWidth / fontScale) / sdfPx;

        setDepthSublayer(0);
        (bucket.*drawSDF)(sdfShader, store, paintMode());
    }

    // Then, we draw the text/icon over the halo
    if (color.a > 0.0f) {
        sdfShader.u_gamma = gamma * gammaScale;
        sdfShader.u_color = color;
        sdfShader.u_opacity = opacity;
        sdfShader.u_buffer = (256.0f - 64.0f) / 256.0f;

        setDepthSublayer(1);
        (bucket.*drawSDF)(sdfShader, store, paintMode());
    }
}

void Painter::renderSymbol(PaintParameters& parameters,
                           SymbolBucket& bucket,
                           const SymbolLayer& layer,
                           const RenderTile& tile) {
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
        if (layout.iconRotationAlignment == AlignmentType::Map) {
            config.depthFunc.reset();
            config.depthTest = GL_TRUE;
        } else {
            config.depthTest = GL_FALSE;
        }

        bool sdf = bucket.sdfIcons;

        const float angleOffset =
            layout.iconRotationAlignment == AlignmentType::Map
                ? state.getAngle()
                : 0;

        const float fontSize = layer.impl->iconSize;
        const float fontScale = fontSize / 1.0f;

        SpriteAtlas* activeSpriteAtlas = layer.impl->spriteAtlas;
        const bool iconScaled = fontScale != 1 || frame.pixelRatio != activeSpriteAtlas->getPixelRatio() || bucket.iconsNeedLinear;
        const bool iconTransformed = layout.iconRotationAlignment == AlignmentType::Map || angleOffset != 0 || state.getPitch() != 0;
        activeSpriteAtlas->bind(sdf || state.isChanging() || iconScaled || iconTransformed, store, config, 0);

        if (sdf) {
            renderSDF(bucket,
                      tile,
                      1.0f,
                      {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }},
                      parameters.shaders.sdfIcon,
                      &SymbolBucket::drawIcons,
                      layout.iconRotationAlignment,
                      // icon-pitch-alignment is not yet implemented
                      // and we simply inherit the rotation alignment
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
            mat4 vtxMatrix = tile.translatedMatrix(paint.iconTranslate,
                                                   paint.iconTranslateAnchor,
                                                   state);

            std::array<float, 2> extrudeScale;

            const bool alignedWithMap = layout.iconRotationAlignment == AlignmentType::Map;
            if (alignedWithMap) {
                extrudeScale.fill(tile.id.pixelsToTileUnits(1, state.getZoom()) * fontScale);
            } else {
                extrudeScale = {{
                    pixelsToGLUnits[0] * fontScale * state.getAltitude(),
                    pixelsToGLUnits[1] * fontScale * state.getAltitude()
                }};
            }

            auto& iconShader = parameters.shaders.icon;

            config.program = iconShader.getID();
            iconShader.u_matrix = vtxMatrix;
            iconShader.u_extrude_scale = extrudeScale;
            iconShader.u_texsize = {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }};
            iconShader.u_rotate_with_map = alignedWithMap;
            iconShader.u_texture = 0;

            // adjust min/max zooms for variable font sies
            float zoomAdjust = std::log(fontSize / layout.iconSize) / std::log(2);
            iconShader.u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level
            iconShader.u_opacity = paint.iconOpacity;

            frameHistory.bind(store, config, 1);
            iconShader.u_fadetexture = 1;

            setDepthSublayer(0);
            bucket.drawIcons(iconShader, store, paintMode());
        }
    }

    if (bucket.hasTextData()) {
        if (layout.textPitchAlignment == AlignmentType::Map) {
            config.depthFunc.reset();
            config.depthTest = GL_TRUE;
        } else {
            config.depthTest = GL_FALSE;
        }

        glyphAtlas->bind(store, config, 0);

        renderSDF(bucket,
                  tile,
                  24.0f,
                  {{ float(glyphAtlas->width) / 4, float(glyphAtlas->height) / 4 }},
                  parameters.shaders.sdfGlyph,
                  &SymbolBucket::drawGlyphs,
                  layout.textRotationAlignment,
                  layout.textPitchAlignment,
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

        auto& collisionBoxShader = shaders->collisionBox;
        config.program = collisionBoxShader.getID();
        collisionBoxShader.u_matrix = tile.matrix;
        // TODO: This was the overscaled z instead of the canonical z.
        collisionBoxShader.u_scale = std::pow(2, state.getZoom() - tile.id.canonical.z);
        collisionBoxShader.u_zoom = state.getZoom() * 10;
        collisionBoxShader.u_maxzoom = (tile.id.canonical.z + 1) * 10;
        config.lineWidth = 1.0f;

        setDepthSublayer(0);
        bucket.drawCollisionBoxes(collisionBoxShader, store);

    }
}

} // namespace mbgl
