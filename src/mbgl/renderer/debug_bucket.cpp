#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/painter.hpp>

#include <mbgl/platform/gl.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace mbgl;

DebugBucket::DebugBucket(DebugFontBuffer& fontBuffer_)
    : fontBuffer(fontBuffer_) {
}

void DebugBucket::render(Painter& painter, util::ptr<StyleLayer> /*layer_desc*/, const Tile::ID& /*id*/, const mat4 &matrix) {
    painter.renderDebugText(*this, matrix);
}

bool DebugBucket::hasData() const {
    return fontBuffer.index() > 0;
}

void DebugBucket::drawLines(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)(fontBuffer.index())));
}

void DebugBucket::drawPoints(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_POINTS, 0, (GLsizei)(fontBuffer.index())));
}
