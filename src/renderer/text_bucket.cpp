#include <llmr/renderer/text_bucket.hpp>
#include <llmr/geometry/text_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/shader-text.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/util/math.hpp>
#include <llmr/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

TextBucket::TextBucket(const std::shared_ptr<TextVertexBuffer>& vertexBuffer,
                       const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
                       const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      vertexBuffer(vertexBuffer),
      triangleElementsBuffer(triangleElementsBuffer),
      vertex_start(vertexBuffer->index()),
      triangle_elements_start(triangleElementsBuffer->index())
{
}

void TextBucket::addGeometry(pbf& geom) {
    // noop
}

void TextBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderText(*this, layer_name, id);
}

bool TextBucket::empty() const {
    return triangleGroups.empty();
}

void TextBucket::drawGlyphs(TextShader& shader) {
    // noop
}
