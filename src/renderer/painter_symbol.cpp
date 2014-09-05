#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/math.hpp>
#include <cmath>

using namespace mbgl;

template <typename BucketProperties, typename StyleProperties>
void Painter::renderSDF(SymbolBucket &bucket,
                        const Tile::ID &id,
                        const mat4 &matrix,
                        const BucketProperties& bucketProperties,
                        const StyleProperties& styleProperties,
                        float sdfFontSize,
                        std::array<float, 2> texsize,
                        SDFShader& sdfShader,
                        void (SymbolBucket::*drawSDF)(SDFShader&))
{
    mat4 vtxMatrix = translatedMatrix(matrix, styleProperties.translate, id, styleProperties.translate_anchor);

    mat4 exMatrix;
    matrix::copy(exMatrix, projMatrix);

    const float angleOffset =
        bucketProperties.rotation_alignment == RotationAlignmentType::Map
            ? map.getState().getAngle()
            : 0;

    if (angleOffset) {
        matrix::rotate_z(exMatrix, exMatrix, angleOffset);
    }

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = std::fmin(styleProperties.size, bucketProperties.max_size);
    float fontScale = fontSize / sdfFontSize;
    matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

    useProgram(sdfShader.program);
    sdfShader.u_matrix = vtxMatrix;
    sdfShader.u_exmatrix = exMatrix;
    sdfShader.u_texsize = texsize;

    // Convert the -pi..pi to an int8 range.
    float angle = std::round(map.getState().getAngle() / M_PI * 128);

    // adjust min/max zooms for variable font sies
    float zoomAdjust = std::log(fontSize / bucketProperties.max_size) / std::log(2);

    sdfShader.u_angle = (int32_t)(angle + 256) % 256;
    sdfShader.u_flip = (bucket.properties.placement == PlacementType::Line ? 1 : 0);
    sdfShader.u_zoom = (map.getState().getNormalizedZoom() - zoomAdjust) * 10; // current zoom level

    FadeProperties f = frameHistory.getFadeProperties(300_milliseconds);
    sdfShader.u_fadedist = f.fadedist * 10;
    sdfShader.u_minfadezoom = std::floor(f.minfadezoom * 10);
    sdfShader.u_maxfadezoom = std::floor(f.maxfadezoom * 10);
    sdfShader.u_fadezoom = (map.getState().getNormalizedZoom() + f.bump) * 10;

    // The default gamma value has to be adjust for the current pixelratio so that we're not
    // drawing blurry font on retina screens.
    const float gamma = 0.105 * sdfFontSize / fontSize / map.getState().getPixelRatio();

    const float sdfPx = 8.0f;
    const float blurOffset = 1.19f;
    const float haloOffset = 6.0f;

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (styleProperties.halo_color[3] > 0.0f) {
        sdfShader.u_gamma = styleProperties.halo_blur * blurOffset / fontScale / sdfPx + gamma;

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

        depthRange(strata, 1.0f);
        (bucket.*drawSDF)(sdfShader);
    }

    // Then, we draw the text/icon over the halo
    if (styleProperties.color[3] > 0.0f) {
        sdfShader.u_gamma = gamma;

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

        depthRange(strata + strata_epsilon, 1.0f);
        (bucket.*drawSDF)(sdfShader);
    }
}

void Painter::renderSymbol(SymbolBucket &bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) {
        return;
    }

    const SymbolProperties &properties = layer_desc->getProperties<SymbolProperties>();

    glDisable(GL_STENCIL_TEST);

    if (bucket.hasTextData()) {
        GlyphAtlas &glyphAtlas = *map.getGlyphAtlas();
        glyphAtlas.bind();

        renderSDF(bucket,
                  id,
                  matrix,
                  bucket.properties.text,
                  properties.text,
                  24.0f,
                  {{ float(glyphAtlas.width) / 4, float(glyphAtlas.height) / 4 }},
                  *sdfGlyphShader,
                  &SymbolBucket::drawGlyphs);
    }

    if (bucket.hasIconData()) {
        bool sdf = bucket.sdfIcons;

        const float angleOffset =
            bucket.properties.icon.rotation_alignment == RotationAlignmentType::Map
                ? map.getState().getAngle()
                : 0;

        // If layerStyle.size > bucket.info.fontSize then labels may collide
        const float fontSize = properties.icon.size != 0 ? properties.icon.size : bucket.properties.icon.max_size;
        const float fontScale = fontSize / 1.0f;

        SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();
        spriteAtlas.bind(map.getState().isChanging() || bucket.properties.placement == PlacementType::Line || angleOffset != 0 || fontScale != 1 || sdf);

        std::array<float, 2> texsize = {{
            float(spriteAtlas.getWidth()),
            float(spriteAtlas.getHeight())
        }};

        if (sdf) {
            renderSDF(bucket,
                      id,
                      matrix,
                      bucket.properties.icon,
                      properties.icon,
                      1.0f,
                      texsize,
                      *sdfIconShader,
                      &SymbolBucket::drawIcons);
        } else {
            mat4 vtxMatrix = translatedMatrix(matrix, properties.icon.translate, id, properties.icon.translate_anchor);

            mat4 exMatrix;
            matrix::copy(exMatrix, projMatrix);

            if (angleOffset) {
                matrix::rotate_z(exMatrix, exMatrix, angleOffset);
            }

            matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

            useProgram(iconShader->program);
            iconShader->u_matrix = vtxMatrix;
            iconShader->u_exmatrix = exMatrix;
            iconShader->u_texsize = texsize;

            // Convert the -pi..pi to an int8 range.
            const float angle = std::round(map.getState().getAngle() / M_PI * 128);

            // adjust min/max zooms for variable font sies
            float zoomAdjust = std::log(fontSize / bucket.properties.icon.max_size) / std::log(2);

            iconShader->u_angle = (int32_t)(angle + 256) % 256;
            iconShader->u_flip = bucket.properties.placement == PlacementType::Line ? 1 : 0;
            iconShader->u_zoom = (map.getState().getNormalizedZoom() - zoomAdjust) * 10; // current zoom level

            iconShader->u_fadedist = 0 * 10;
            iconShader->u_minfadezoom = map.getState().getNormalizedZoom() * 10;
            iconShader->u_maxfadezoom = map.getState().getNormalizedZoom() * 10;
            iconShader->u_fadezoom = map.getState().getNormalizedZoom() * 10;
            iconShader->u_opacity = properties.icon.opacity;

            depthRange(strata, 1.0f);
            bucket.drawIcons(*iconShader);
        }
    }

    glEnable(GL_STENCIL_TEST);
}
