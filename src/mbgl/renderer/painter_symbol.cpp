#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

using namespace mbgl;

void Painter::renderSDF(SymbolBucket &bucket,
                        const UnwrappedTileID &tileID,
                        const mat4 &matrix,
                        float sdfFontSize,
                        std::array<float, 2> texsize,
                        SDFShader& sdfShader,
                        void (SymbolBucket::*drawSDF)(SDFShader&, gl::GLObjectStore&),

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

    bool skewed = rotationAlignment == RotationAlignmentType::Map;
    mat4 exMatrix;
    float s;
    float gammaScale;

    if (skewed) {
        matrix::identity(exMatrix);
        s = tileID.pixelsToTileUnits(1, state.getZoom());
        gammaScale = 1.0f / std::cos(state.getPitch());
    } else {
        exMatrix = extrudeMatrix;
        s = state.getAltitude();
        gammaScale = 1.0f;
        matrix::rotate_z(exMatrix, exMatrix, state.getNorthOrientationAngle());
    }
    const bool flippedY = !skewed && state.getViewportMode() == ViewportMode::FlippedY;
    matrix::scale(exMatrix, exMatrix, s, flippedY ? -s : s, 1);

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = paintSize;
    float fontScale = fontSize / sdfFontSize;
    matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

    config.program = sdfShader.getID();
    sdfShader.u_matrix = vtxMatrix;
    sdfShader.u_exmatrix = exMatrix;
    sdfShader.u_texsize = texsize;
    sdfShader.u_skewed = skewed;
    sdfShader.u_texture = 0;

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(fontSize / layoutSize) / std::log(2);

    sdfShader.u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level

    config.activeTexture = GL_TEXTURE1;
    frameHistory.bind(glObjectStore);
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
        (bucket.*drawSDF)(sdfShader, glObjectStore);
    }

    // Then, we draw the text/icon over the halo
    if (color[3] > 0.0f) {
        sdfShader.u_gamma = gamma * gammaScale;
        sdfShader.u_color = color;
        sdfShader.u_opacity = opacity;
        sdfShader.u_buffer = (256.0f - 64.0f) / 256.0f;

        setDepthSublayer(1);
        (bucket.*drawSDF)(sdfShader, glObjectStore);
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

    const auto& paint = layer.paint;
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

        const float fontSize = layer.iconSize;
        const float fontScale = fontSize / 1.0f;

        SpriteAtlas* activeSpriteAtlas = layer.spriteAtlas;
        const bool iconScaled = fontScale != 1 || frame.pixelRatio != activeSpriteAtlas->getPixelRatio() || bucket.iconsNeedLinear;
        const bool iconTransformed = layout.iconRotationAlignment == RotationAlignmentType::Map || angleOffset != 0 || state.getPitch() != 0;
        config.activeTexture = GL_TEXTURE0;
        activeSpriteAtlas->bind(sdf || state.isChanging() || iconScaled || iconTransformed, glObjectStore);

        if (sdf) {
            renderSDF(bucket,
                      tileID,
                      matrix,
                      1.0f,
                      {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }},
                      *sdfIconShader,
                      &SymbolBucket::drawIcons,
                      layout.iconRotationAlignment,
                      layout.iconSize,
                      paint.iconOpacity,
                      paint.iconColor,
                      paint.iconHaloColor,
                      paint.iconHaloWidth,
                      paint.iconHaloBlur,
                      paint.iconTranslate,
                      paint.iconTranslateAnchor,
                      layer.iconSize);
        } else {
            mat4 vtxMatrix =
                translatedMatrix(matrix, paint.iconTranslate, tileID, paint.iconTranslateAnchor);

            bool skewed = layout.iconRotationAlignment == RotationAlignmentType::Map;
            mat4 exMatrix;
            float s;

            if (skewed) {
                matrix::identity(exMatrix);
                s = tileID.pixelsToTileUnits(1, state.getZoom());
            } else {
                exMatrix = extrudeMatrix;
                matrix::rotate_z(exMatrix, exMatrix, state.getNorthOrientationAngle());
                s = state.getAltitude();
            }
            const bool flippedY = !skewed && state.getViewportMode() == ViewportMode::FlippedY;
            matrix::scale(exMatrix, exMatrix, s, flippedY ? -s : s, 1);

            matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

            // calculate how much longer the real world distance is at the top of the screen
            // than at the middle of the screen.
            float topedgelength = std::sqrt(std::pow(state.getHeight(), 2) / 4.0f * (1.0f + std::pow(state.getAltitude(), 2)));
            float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
            float extra = (topedgelength + x) / topedgelength - 1;

            config.program = iconShader->getID();
            iconShader->u_matrix = vtxMatrix;
            iconShader->u_exmatrix = exMatrix;
            iconShader->u_texsize = {{ float(activeSpriteAtlas->getWidth()) / 4.0f, float(activeSpriteAtlas->getHeight()) / 4.0f }};
            iconShader->u_skewed = skewed;
            iconShader->u_extra = extra;
            iconShader->u_texture = 0;

            // adjust min/max zooms for variable font sies
            float zoomAdjust = std::log(fontSize / layout.iconSize) / std::log(2);
            iconShader->u_zoom = (state.getZoom() - zoomAdjust) * 10; // current zoom level
            iconShader->u_opacity = paint.iconOpacity;

            config.activeTexture = GL_TEXTURE1;
            frameHistory.bind(glObjectStore);
            iconShader->u_fadetexture = 1;

            setDepthSublayer(0);
            bucket.drawIcons(*iconShader, glObjectStore);
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
        glyphAtlas->bind(glObjectStore);

        renderSDF(bucket,
                  tileID,
                  matrix,
                  24.0f,
                  {{ float(glyphAtlas->width) / 4, float(glyphAtlas->height) / 4 }},
                  *sdfGlyphShader,
                  &SymbolBucket::drawGlyphs,
                  layout.textRotationAlignment,
                  layout.textSize,
                  paint.textOpacity,
                  paint.textColor,
                  paint.textHaloColor,
                  paint.textHaloWidth,
                  paint.textHaloBlur,
                  paint.textTranslate,
                  paint.textTranslateAnchor,
                  layer.textSize);
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
        bucket.drawCollisionBoxes(*collisionBoxShader, glObjectStore);

    }

    config.activeTexture = GL_TEXTURE0;
}
