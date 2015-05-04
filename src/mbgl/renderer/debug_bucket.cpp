#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>

#include <mbgl/platform/gl.hpp>

#include <cassert>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

using namespace mbgl;

DebugBucket::DebugBucket(DebugFontBuffer& fontBuffer_)
    : fontBuffer(fontBuffer_) {
}

void DebugBucket::upload() {
    fontBuffer.upload();

    uploaded = true;
}

void DebugBucket::render(Painter& painter, const StyleLayer&, const TileID&, const mat4& matrix) {
    painter.renderDebugText(*this, matrix);
}

void DebugBucket::drawLines(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)(fontBuffer.index())));
}

void DebugBucket::drawPoints(PlainShader& shader) {
    array.bind(shader, fontBuffer, BUFFER_OFFSET(0));
    MBGL_CHECK_ERROR(glDrawArrays(GL_POINTS, 0, (GLsizei)(fontBuffer.index())));
}
