#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>

#include <mbgl/platform/gl.hpp>

#include <cassert>
#include <string>

using namespace mbgl;

DebugBucket::DebugBucket(const TileID id, const TileData::State state_) : state(state_) {
    const std::string text = std::string(id) + " - " + TileData::StateToString(state);
    fontBuffer.addText(text.c_str(), 50, 200, 5);
}

void DebugBucket::drawLines(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET_0);
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)(fontBuffer.index())));
}

void DebugBucket::drawPoints(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET_0);
    MBGL_CHECK_ERROR(glDrawArrays(GL_POINTS, 0, (GLsizei)(fontBuffer.index())));
}
