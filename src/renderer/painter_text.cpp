#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/map/map.hpp>

using namespace llmr;

void Painter::renderText(TextBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (pass == Opaque) return;
    if (!bucket.hasData()) return;

    auto text_properties = map.getStyle().computed.texts;
    auto text_properties_it = text_properties.find(layer_name);
    if (text_properties_it == text_properties.end()) return;

    const TextProperties& properties = text_properties_it->second;
    if (!properties.enabled) return;

    mat4 exMatrix;
    matrix::copy(exMatrix, projMatrix);
    if (bucket.geom_desc.path == TextPathType::Curve) {
        matrix::rotate_z(exMatrix, exMatrix, map.getState().getAngle());
    }

    const float rotate = properties.rotate;
    if (rotate != 0.0f) {
        matrix::rotate_z(exMatrix, exMatrix, rotate);
    }

    // If layerStyle.size > bucket.info.fontSize then labels may collide
    float fontSize = std::fmin(properties.size, bucket.geom_desc.size);
    matrix::scale(exMatrix, exMatrix, fontSize / 24.0f, fontSize / 24.0f, 1.0f);

    const mat4 &vtxMatrix = translatedMatrix(properties.translate, id, properties.translateAnchor);

    useProgram(textShader->program);
    textShader->setMatrix(vtxMatrix);
    textShader->setExtrudeMatrix(exMatrix);

    map.getGlyphAtlas().bind();
    textShader->setTextureSize({{static_cast<float>(map.getGlyphAtlas().width),
                                 static_cast<float>(map.getGlyphAtlas().height)}});

    textShader->setGamma(2.5f / fontSize / map.getState().getPixelRatio());

    // Convert the -pi..pi to an int8 range.
    float angle = std::round((map.getState().getAngle() + rotate) / M_PI * 128);

    // adjust min/max zooms for variable font sies
    float zoomAdjust = log(fontSize / bucket.geom_desc.size) / log(2);

    textShader->setAngle((int32_t)(angle + 256) % 256);
    textShader->setFlip(bucket.geom_desc.path == TextPathType::Curve ? 1 : 0);
    textShader->setZoom((map.getState().getNormalizedZoom() - zoomAdjust) * 10); // current zoom level

    // Label fading
    const time duration = 300_milliseconds;
    const time currentTime = util::now();

    std::deque<FrameSnapshot> &history = frameHistory.history;

    // Remove frames until only one is outside the duration, or until there are only three
    while (history.size() > 3 && history[1].timestamp + duration < currentTime) {
        history.pop_front();
    }

    if (history[1].timestamp + duration < currentTime) {
        history[0].z = history[1].z;
    }

    size_t frameLen = history.size();
    assert("there should never be less than three frames in the history" && frameLen >= 3);

    // Find the range of zoom levels we want to fade between
    float startingZ = history.front().z;
    const FrameSnapshot lastFrame = history.back();
    float endingZ = lastFrame.z;
    float lowZ = std::fmin(startingZ, endingZ);
    float highZ = std::fmax(startingZ, endingZ);

    // Calculate the speed of zooming, and how far it would zoom in terms of zoom levels in one duration
    float zoomDiff = endingZ - history[1].z,
        timeDiff = lastFrame.timestamp - history[1].timestamp;
    if (timeDiff > duration) timeDiff = 1;
    float fadedist = zoomDiff / (timeDiff / duration);

    if (isnan(fadedist)) fprintf(stderr, "fadedist should never be NaN\n");

    // At end of a zoom when the zoom stops changing continue pretending to zoom at that speed
    // bump is how much farther it would have been if it had continued zooming at the same rate
    float bump = (currentTime - lastFrame.timestamp) / duration * fadedist;

    textShader->setFadeDist(fadedist * 10);
    textShader->setMinFadeZoom(std::floor(lowZ * 10));
    textShader->setMaxFadeZoom(std::floor(highZ * 10));
    textShader->setFadeZoom((map.getState().getZoom() + bump) * 10);

    // We're drawing in the translucent pass which is bottom-to-top, so we need
    // to draw the halo first.
    if (properties.halo[3] > 0.0f) {
        textShader->setColor(properties.halo);
        textShader->setBuffer(properties.haloRadius);
        glDepthRange(strata, 1.0f);
        bucket.drawGlyphs(*textShader);
    }

    // Then, we draw the text over the halo
    textShader->setColor(properties.color);
    textShader->setBuffer((256.0f - 64.0f) / 256.0f);
    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawGlyphs(*textShader);
}
