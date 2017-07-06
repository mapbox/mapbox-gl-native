#pragma once

#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>

namespace mbgl {

template <class DrawMode, class LayoutVertex, class AttributeLayout>
class IndexedPrimitives;

template <class DrawMode, class LayoutVertex, class Attributes>
class Drawable {
public:
    Drawable(gl::Context& context,
             IndexedPrimitives<DrawMode, LayoutVertex, Attributes>&& primitives)
        : vertices(context.createVertexBuffer(std::move(primitives.vertices))),
          indices(context.createIndexBuffer(std::move(primitives.indices))),
          segments(primitives.segmentInfo.begin(), primitives.segmentInfo.end()) {
        primitives = {};
    }

    const gl::VertexBuffer<LayoutVertex> vertices;
    const gl::IndexBuffer<DrawMode> indices;
    const gl::SegmentVector<Attributes> segments;
};

} // namespace mbgl
