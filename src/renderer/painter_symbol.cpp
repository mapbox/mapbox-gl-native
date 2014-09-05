#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/math.hpp>
#include <cmath>

namespace mbgl {

void Painter::renderSymbol(SymbolBucket &bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) {
        return;
    }

    const SymbolProperties &properties = layer_desc->getProperties<SymbolProperties>();

    glDisable(GL_STENCIL_TEST);

    if (bucket.hasTextData()) {
        mat4 vtxMatrix = translatedMatrix(matrix, properties.text.translate, id, properties.text.translate_anchor);

        mat4 exMatrix;
        matrix::copy(exMatrix, projMatrix);

        const float angleOffset =
            bucket.properties.text.rotation_alignment == RotationAlignmentType::Map
                ? map.getState().getAngle()
                : 0;

        if (angleOffset) {
            matrix::rotate_z(exMatrix, exMatrix, angleOffset);
        }

        if (bucket.properties.placement == PlacementType::Line) {
            matrix::rotate_z(exMatrix, exMatrix, map.getState().getAngle());
        }

        // If layerStyle.size > bucket.info.fontSize then labels may collide
        float fontSize = std::fmin(properties.text.size, bucket.properties.text.max_size);
        matrix::scale(exMatrix, exMatrix, fontSize / 24.0f, fontSize / 24.0f, 1.0f);

        useProgram(sdfShader->program);
        sdfShader->u_matrix = vtxMatrix;
        sdfShader->u_exmatrix = exMatrix;

        GlyphAtlas &glyphAtlas = *map.getGlyphAtlas();
        glyphAtlas.bind();
        sdfShader->u_texsize = {{
            static_cast<float>(glyphAtlas.width),
            static_cast<float>(glyphAtlas.height)
        }};

        // Convert the -pi..pi to an int8 range.
        float angle = std::round((map.getState().getAngle()) / M_PI * 128);

        // adjust min/max zooms for variable font sies
        float zoomAdjust = log(fontSize / bucket.properties.text.max_size) / log(2);

        sdfShader->u_angle = (int32_t)(angle + 256) % 256;
        sdfShader->u_flip = (bucket.properties.placement == PlacementType::Line ? 1 : 0);
        sdfShader->u_zoom = (map.getState().getNormalizedZoom() - zoomAdjust) * 10; // current zoom level

        FadeProperties f = frameHistory.getFadeProperties(300_milliseconds);
        sdfShader->u_fadedist = f.fadedist * 10;
        sdfShader->u_minfadezoom = std::floor(f.minfadezoom * 10);
        sdfShader->u_maxfadezoom = std::floor(f.maxfadezoom * 10);
        sdfShader->u_fadezoom = (map.getState().getNormalizedZoom() + f.bump) * 10;

        // This defines the gamma around the SDF cutoff value.
        const float sdfGamma = 1.0f / 10.0f;

        // Our signed distance fields are scaled so that 1 pixel is scaled to 32 pixels.
        // Our cutoff between positive and negative values is hard coded to 64 (== 2 pixels).
        // This means that our 6/8 of the value range lies outside the glyph outline.
        const float sdfOffset = (256.0f - 64.0f) / 32.0f;

        // Currently, all of our fonts are rendered with a font size of 24px.
        const float sdfFontSize = 24.0f;

        // The default gamma value has to be adjust for the current pixelratio so that we're not
        // drawing
        // blurry font on retina screens.
        const float gamma = sdfGamma * sdfFontSize / fontSize / map.getState().getPixelRatio();

        // We're drawing in the translucent pass which is bottom-to-top, so we need
        // to draw the halo first.
        if (properties.text.halo_color[3] > 0.0f) {
            const float haloWidth = util::clamp(
                (sdfOffset - properties.text.halo_width / (fontSize / sdfFontSize)) / 8.0f, 0.0f,
                1.0f);

            if (properties.text.halo_blur != 0.0f) {
                // We are converting the halo_blur value to current screen pixels.
                // Then we're dividing it by two because the gamma value is added/subtracted into
                // both
                // directions in the shader, but the halo_blur describes the entire width of the
                // blur.
                // Note that this does *not* have to be adjusted for retina screens, because we want
                // the
                // same blur width when we explicitly specify one.
                sdfShader->u_gamma = (properties.text.halo_blur / (fontSize / sdfFontSize)) / 8.0f / 2.0f;
            } else {
                sdfShader->u_gamma = sdfGamma;
            }

            if (properties.text.opacity < 1.0f) {
                Color color = properties.text.halo_color;
                color[0] *= properties.text.opacity;
                color[1] *= properties.text.opacity;
                color[2] *= properties.text.opacity;
                color[3] *= properties.text.opacity;
                sdfShader->u_color = color;
            } else {
                sdfShader->u_color = properties.text.halo_color;
            }
            sdfShader->u_buffer = haloWidth;
            depthRange(strata, 1.0f);
            bucket.drawGlyphs(*sdfShader);
        }

        if (properties.text.color[3] > 0.0f) {
            // Then, we draw the text over the halo
            sdfShader->u_gamma = gamma;
            if (properties.text.opacity < 1.0f) {
                Color color = properties.text.color;
                color[0] *= properties.text.opacity;
                color[1] *= properties.text.opacity;
                color[2] *= properties.text.opacity;
                color[3] *= properties.text.opacity;
                sdfShader->u_color = color;
            } else {
                sdfShader->u_color = properties.text.color;
            }
            sdfShader->u_buffer = (256.0f - 64.0f) / 256.0f;
            depthRange(strata + strata_epsilon, 1.0f);
            bucket.drawGlyphs(*sdfShader);
        }
    }

    if (bucket.hasIconData()) {
        mat4 vtxMatrix = translatedMatrix(matrix, properties.icon.translate, id, properties.icon.translate_anchor);

        mat4 exMatrix;
        matrix::copy(exMatrix, projMatrix);

        const float angleOffset =
            bucket.properties.icon.rotation_alignment == RotationAlignmentType::Map
                ? map.getState().getAngle()
                : 0;

        if (angleOffset) {
            matrix::rotate_z(exMatrix, exMatrix, angleOffset);
        }

        // If layerStyle.size > bucket.info.fontSize then labels may collide
        const float fontSize = properties.icon.size != 0 ? properties.icon.size : bucket.properties.icon.max_size;
        const float fontScale = fontSize / 1.0f;
        matrix::scale(exMatrix, exMatrix, fontScale, fontScale, 1.0f);

        useProgram(iconShader->program);
        iconShader->u_matrix = vtxMatrix;
        iconShader->u_exmatrix = exMatrix;

        SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();
        spriteAtlas.bind(map.getState().isChanging() || bucket.properties.placement == PlacementType::Line || angleOffset != 0 || fontScale != 1);
        iconShader->u_texsize = {{
            static_cast<float>(spriteAtlas.getWidth()),
            static_cast<float>(spriteAtlas.getHeight())
        }};

        // Convert the -pi..pi to an int8 range.
        const float angle = std::round((map.getState().getAngle()) / M_PI * 128);

        // adjust min/max zooms for variable font sies
        float zoomAdjust = log(fontSize / bucket.properties.icon.max_size) / log(2);

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

    glEnable(GL_STENCIL_TEST);
}
}
