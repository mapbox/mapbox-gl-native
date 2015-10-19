#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/box_shader.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

using namespace mbgl;

template <typename BucketProperties, typename StyleProperties>
void Painter::renderSDF(SymbolBucket &bucket,
                        const TileID &id,
                        const mat4 &matrix,
                        const BucketProperties& bucketProperties,
                        const StyleProperties& styleProperties,
                        float sdfFontSize,
                        std::array<float, 2> texsize,
                        SDFShader& sdfShader,
                        void (SymbolBucket::*drawSDF)(SDFShader&))
{
    mat4 vtxMatrix = translatedMatrix(matrix, styleProperties.translate, id, styleProperties.translate_anchor);

    bool aligned_with_map = (bucketProperties.rotation_alignment == RotationAlignmentType::Map);
    bool skewed = aligned_with_map;
    mat4 exMatrix;
    float s;
    float gammaScale;

    if (skewed) {
        matrix::identity(exMatrix);
        s = 4096.0f / util::tileSize / id.overscaling / std::pow(2, state.getZoom() - id.z);
        gammaScale = 1.0f / std::cos(state.getPitch());
    } else {
        exMatrix = extrudeMatrix;
        s = state.getAltitude();
        gammaScale = 1.0f;
    }
    matrix::scale(exMatrix, exMatrix, s, s, 1);

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = styleProperties.size;
    float fontScale = fontSize / sdfFontSize;
    matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

    // calculate how much longer the real world distance is at the top of the screen
    // than at the middle of the screen.
    float topedgelength = std::sqrt(std::pow(state.getHeight(), 2) / 4.0f * (1.0f + std::pow(state.getAltitude(), 2)));
    float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
    float extra = (topedgelength + x) / topedgelength - 1;

    useProgram(sdfShader.program);
    sdfShader.u_matrix = vtxMatrix;
    sdfShader.u_exmatrix = exMatrix;
    sdfShader.u_texsize = texsize;
    sdfShader.u_skewed = skewed;
    sdfShader.u_extra = extra;

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(fontSize / bucketProperties.size) / std::log(2);

    sdfShader.u_zoom = (state.getNormalizedZoom() - zoomAdjust) * 10; // current zoom level

    FadeProperties f = frameHistory.getFadeProperties(data.getAnimationTime(), data.getDefaultFadeDuration());
    sdfShader.u_fadedist = f.fadedist * 10;
    sdfShader.u_minfadezoom = std::floor(f.minfadezoom * 10);
    sdfShader.u_maxfadezoom = std::floor(f.maxfadezoom * 10);
    sdfShader.u_fadezoom = (state.getNormalizedZoom() + f.bump) * 10;

    // The default gamma value has to be adjust for the current pixelratio so that we're not
    // drawing blurry font on retina screens.
    const float gamma = 0.105 * sdfFontSize / fontSize / data.pixelRatio;

    const float sdfPx = 8.0f;
    const float blurOffset = 1.19f;
    const float haloOffset = 6.0f;

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (styleProperties.halo_color[3] > 0.0f && styleProperties.halo_width > 0.0f) {
        sdfShader.u_gamma = (styleProperties.halo_blur * blurOffset / fontScale / sdfPx + gamma) * gammaScale;

        if (styleProperties.opacity < 1.0f) {
            Color color = styleProperties.halo_color;
            color[0] *= styleProperties.opacity;
            color[1] *= styleProperties.opacity;
            color[2] *= styleProperties.opacity;
            color[3] *= styleProperties.opacity;
            sdfShader.u_color = color;
        } else {
            sdfShader.u_color = styleProperties.halo_color;
        }

        sdfShader.u_buffer = (haloOffset - styleProperties.halo_width / fontScale) / sdfPx;

        setDepthSublayer(0);
        (bucket.*drawSDF)(sdfShader);
    }

    // Then, we draw the text/icon over the halo
    if (styleProperties.color[3] > 0.0f) {
        sdfShader.u_gamma = gamma * gammaScale;

        if (styleProperties.opacity < 1.0f) {
            Color color = styleProperties.color;
            color[0] *= styleProperties.opacity;
            color[1] *= styleProperties.opacity;
            color[2] *= styleProperties.opacity;
            color[3] *= styleProperties.opacity;
            sdfShader.u_color = color;
        } else {
            sdfShader.u_color = styleProperties.color;
        }

        sdfShader.u_buffer = (256.0f - 64.0f) / 256.0f;

        setDepthSublayer(1);
        (bucket.*drawSDF)(sdfShader);
    }
}

void Painter::renderSymbol(SymbolBucket& bucket, const SymbolLayer& layer, const TileID& id, const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto& properties = layer.properties;
    const auto& layout = bucket.layout;

    config.depthMask = GL_FALSE;

    if (bucket.hasCollisionBoxData()) {
        config.stencilTest = true;

        useProgram(collisionBoxShader->program);
        collisionBoxShader->u_matrix = matrix;
        collisionBoxShader->u_scale = std::pow(2, state.getNormalizedZoom() - id.z);
        collisionBoxShader->u_zoom = state.getNormalizedZoom() * 10;
        collisionBoxShader->u_maxzoom = (id.z + 1) * 10;
        lineWidth(1.0f);

        setDepthSublayer(0);
        bucket.drawCollisionBoxes(*collisionBoxShader);

    }

    // TODO remove the `|| true` when #1673 is implemented
    const bool drawAcrossEdges = !(layout.text.allow_overlap || layout.icon.allow_overlap ||
          layout.text.ignore_placement || layout.icon.ignore_placement) || true;

    // Disable the stencil test so that labels aren't clipped to tile boundaries.
    //
    // Layers with features that may be drawn overlapping aren't clipped. These
    // layers are sorted in the y direction, and to draw the correct ordering near
    // tile edges the icons are included in both tiles and clipped when drawing.
    config.stencilTest = drawAcrossEdges ? false : true;

    if (bucket.hasIconData()) {
        config.depthTest = layout.icon.rotation_alignment == RotationAlignmentType::Map;

        bool sdf = bucket.sdfIcons;

        const float angleOffset =
            layout.icon.rotation_alignment == RotationAlignmentType::Map
                ? state.getAngle()
                : 0;

        const float fontSize = properties.icon.size;
        const float fontScale = fontSize / 1.0f;

        spriteAtlas->bind(state.isChanging() || layout.placement == PlacementType::Line
                || angleOffset != 0 || fontScale != 1 || sdf || state.getPitch() != 0);

        if (sdf) {
            renderSDF(bucket,
                      id,
                      matrix,
                      layout.icon,
                      properties.icon,
                      1.0f,
                      {{ float(spriteAtlas->getWidth()) / 4.0f, float(spriteAtlas->getHeight()) / 4.0f }},
                      *sdfIconShader,
                      &SymbolBucket::drawIcons);
        } else {
            mat4 vtxMatrix = translatedMatrix(matrix, properties.icon.translate, id, properties.icon.translate_anchor);

            bool skewed = layout.icon.rotation_alignment == RotationAlignmentType::Map;
            mat4 exMatrix;
            float s;

            if (skewed) {
                matrix::identity(exMatrix);
                s = 4096.0f / util::tileSize / id.overscaling / std::pow(2, state.getZoom() - id.z);
            } else {
                exMatrix = extrudeMatrix;
                s = state.getAltitude();
            }
            matrix::scale(exMatrix, exMatrix, s, s, 1);

            matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

            // calculate how much longer the real world distance is at the top of the screen
            // than at the middle of the screen.
            float topedgelength = std::sqrt(std::pow(state.getHeight(), 2) / 4.0f * (1.0f + std::pow(state.getAltitude(), 2)));
            float x = state.getHeight() / 2.0f * std::tan(state.getPitch());
            float extra = (topedgelength + x) / topedgelength - 1;

            useProgram(iconShader->program);
            iconShader->u_matrix = vtxMatrix;
            iconShader->u_exmatrix = exMatrix;
            iconShader->u_texsize = {{ float(spriteAtlas->getWidth()) / 4.0f, float(spriteAtlas->getHeight()) / 4.0f }};
            iconShader->u_skewed = skewed;
            iconShader->u_extra = extra;

            // adjust min/max zooms for variable font sies
            float zoomAdjust = std::log(fontSize / layout.icon.size) / std::log(2);

            iconShader->u_zoom = (state.getNormalizedZoom() - zoomAdjust) * 10; // current zoom level
            iconShader->u_fadedist = 0 * 10;
            iconShader->u_minfadezoom = state.getNormalizedZoom() * 10;
            iconShader->u_maxfadezoom = state.getNormalizedZoom() * 10;
            iconShader->u_fadezoom = state.getNormalizedZoom() * 10;
            iconShader->u_opacity = properties.icon.opacity;

            setDepthSublayer(0);
            bucket.drawIcons(*iconShader);
        }
    }

    if (bucket.hasTextData()) {
        config.depthTest = layout.text.rotation_alignment == RotationAlignmentType::Map;

        glyphAtlas->bind();

        renderSDF(bucket,
                  id,
                  matrix,
                  layout.text,
                  properties.text,
                  24.0f,
                  {{ float(glyphAtlas->width) / 4, float(glyphAtlas->height) / 4 }},
                  *sdfGlyphShader,
                  &SymbolBucket::drawGlyphs);
    }

}
