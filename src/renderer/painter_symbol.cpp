#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/math.hpp>
#include <cmath>

namespace mbgl {

void Painter::renderSymbol(SymbolBucket &bucket, std::shared_ptr<StyleLayer> layer_desc,
                           const Tile::ID &/*id*/, const mat4 &matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) {
        return;
    }

    const SymbolProperties &properties = layer_desc->getProperties<SymbolProperties>();

    glDisable(GL_STENCIL_TEST);

    if (bucket.hasTextData()) {
        mat4 exMatrix;
        matrix::copy(exMatrix, projMatrix);
        if (bucket.properties.placement == PlacementType::Line) {
            matrix::rotate_z(exMatrix, exMatrix, map.getState().getAngle());
        }

        // If layerStyle.size > bucket.info.fontSize then labels may collide
        float fontSize = std::fmin(properties.text.size, bucket.properties.text.max_size);
        matrix::scale(exMatrix, exMatrix, fontSize / 24.0f, fontSize / 24.0f, 1.0f);

        useProgram(textShader->program);
        textShader->setMatrix(matrix);
        textShader->setExtrudeMatrix(exMatrix);

        GlyphAtlas &glyphAtlas = *map.getGlyphAtlas();
        glyphAtlas.bind();
        textShader->setTextureSize(
            {{static_cast<float>(glyphAtlas.width), static_cast<float>(glyphAtlas.height)}});

        // Convert the -pi..pi to an int8 range.
        float angle = std::round((map.getState().getAngle()) / M_PI * 128);

        // adjust min/max zooms for variable font sies
        float zoomAdjust = log(fontSize / bucket.properties.text.max_size) / log(2);

        textShader->setAngle((int32_t)(angle + 256) % 256);
        textShader->setFlip(bucket.properties.placement == PlacementType::Line ? 1 : 0);
        textShader->setZoom((map.getState().getNormalizedZoom() - zoomAdjust) *
                            10); // current zoom level

        // Label fading
        const timestamp duration = 300_milliseconds;
        const timestamp currentTime = util::now();

        std::deque<FrameSnapshot> &history = frameHistory.history;

        // Remove frames until only one is outside the duration, or until there are only three
        while (history.size() > 3 && history[1].t + duration < currentTime) {
            history.pop_front();
        }

        if (history[1].t + duration < currentTime) {
            history[0].z = history[1].z;
        }

        assert("there should never be less than three frames in the history" &&
               (history.size() >= 3));

        // Find the range of zoom levels we want to fade between
        float startingZ = history.front().z;
        const FrameSnapshot lastFrame = history.back();
        float endingZ = lastFrame.z;
        float lowZ = std::fmin(startingZ, endingZ);
        float highZ = std::fmax(startingZ, endingZ);

        // Calculate the speed of zooming, and how far it would zoom in terms of zoom levels in one
        // duration
        float zoomDiff = endingZ - history[1].z, timeDiff = lastFrame.t - history[1].t;
        float fadedist = zoomDiff / (timeDiff / duration);

#if defined(DEBUG)
//        if (std::isnan(fadedist))
//            fprintf(stderr, "fadedist should never be NaN\n");
#endif

        // At end of a zoom when the zoom stops changing continue pretending to zoom at that speed
        // bump is how much farther it would have been if it had continued zooming at the same rate
        float bump = (currentTime - lastFrame.t) / duration * fadedist;

        textShader->setFadeDist(fadedist * 10);
        textShader->setMinFadeZoom(std::floor(lowZ * 10));
        textShader->setMaxFadeZoom(std::floor(highZ * 10));
        textShader->setFadeZoom((map.getState().getNormalizedZoom() + bump) * 10);

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
                textShader->setGamma((properties.text.halo_blur / (fontSize / sdfFontSize)) / 8.0f /
                                     2.0f);
            } else {
                textShader->setGamma(sdfGamma);
            }

            if (properties.text.opacity < 1.0f) {
                Color color = properties.text.halo_color;
                color[0] *= properties.text.opacity;
                color[1] *= properties.text.opacity;
                color[2] *= properties.text.opacity;
                color[3] *= properties.text.opacity;
                textShader->setColor(color);
            } else {
                textShader->setColor(properties.text.halo_color);
            }
            textShader->setBuffer(haloWidth);
            depthRange(strata, 1.0f);
            bucket.drawGlyphs(*textShader);
        }

        if (properties.text.color[3] > 0.0f) {
            // Then, we draw the text over the halo
            textShader->setGamma(gamma);
            if (properties.text.opacity < 1.0f) {
                Color color = properties.text.color;
                color[0] *= properties.text.opacity;
                color[1] *= properties.text.opacity;
                color[2] *= properties.text.opacity;
                color[3] *= properties.text.opacity;
                textShader->setColor(color);
            } else {
                textShader->setColor(properties.text.color);
            }
            textShader->setBuffer((256.0f - 64.0f) / 256.0f);
            depthRange(strata + strata_epsilon, 1.0f);
            bucket.drawGlyphs(*textShader);
        }
    }

    if (bucket.hasIconData()) {
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
        iconShader->setMatrix(matrix);
        iconShader->setExtrudeMatrix(exMatrix);

        SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();
        spriteAtlas.bind(map.getState().isChanging() || bucket.properties.placement == PlacementType::Line || angleOffset != 0 || fontScale != 1);
        iconShader->setTextureSize(
            {{static_cast<float>(spriteAtlas.getWidth()), static_cast<float>(spriteAtlas.getHeight())}});

        // Convert the -pi..pi to an int8 range.
        const float angle = std::round((map.getState().getAngle()) / M_PI * 128);

        // adjust min/max zooms for variable font sies
        float zoomAdjust = log(fontSize / bucket.properties.icon.max_size) / log(2);

        iconShader->setAngle((int32_t)(angle + 256) % 256);
        iconShader->setFlip(bucket.properties.placement == PlacementType::Line ? 1 : 0);
        iconShader->setZoom((map.getState().getNormalizedZoom() - zoomAdjust) *
                            10); // current zoom level

        iconShader->setFadeDist(0 * 10);
        iconShader->setMinFadeZoom(map.getState().getNormalizedZoom() * 10);
        iconShader->setMaxFadeZoom(map.getState().getNormalizedZoom() * 10);
        iconShader->setFadeZoom(map.getState().getNormalizedZoom() * 10);
        iconShader->setOpacity(properties.icon.opacity);

        depthRange(strata, 1.0f);
        bucket.drawIcons(*iconShader);
    }

    glEnable(GL_STENCIL_TEST);
}
}
