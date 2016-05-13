#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/gl/gl.hpp>

#include <cassert>
#include <string>

using namespace mbgl;

DebugBucket::DebugBucket(const OverscaledTileID& id,
                         const TileData::State state_,
                         optional<Timestamp> modified_,
                         optional<Timestamp> expires_,
                         MapDebugOptions debugMode_)
    : state(state_),
      modified(std::move(modified_)),
      expires(std::move(expires_)),
      debugMode(debugMode_) {
    double baseline = 200;
    if (debugMode & MapDebugOptions::ParseStatus) {
        const std::string text = util::toString(id) + " - " + TileData::StateToString(state);
        fontBuffer.addText(text.c_str(), 50, baseline, 5);
        baseline += 200;
    }

    if (debugMode & MapDebugOptions::Timestamps && modified && expires) {
        const std::string modifiedText = "modified: " + util::iso8601(*modified);
        fontBuffer.addText(modifiedText.c_str(), 50, baseline, 5);

        const std::string expiresText = "expires: " + util::iso8601(*expires);
        fontBuffer.addText(expiresText.c_str(), 50, baseline + 200, 5);
    }
}

void DebugBucket::drawLines(PlainShader& shader, gl::GLObjectStore& glObjectStore) {
    if (!fontBuffer.empty()) {
        array.bind(shader, fontBuffer, BUFFER_OFFSET_0, glObjectStore);
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)(fontBuffer.index())));
    }
}

void DebugBucket::drawPoints(PlainShader& shader, gl::GLObjectStore& glObjectStore) {
    if (!fontBuffer.empty()) {
        array.bind(shader, fontBuffer, BUFFER_OFFSET_0, glObjectStore);
        MBGL_CHECK_ERROR(glDrawArrays(GL_POINTS, 0, (GLsizei)(fontBuffer.index())));
    }
}
