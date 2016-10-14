#pragma once

#include <mbgl/util/size.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/shader/fill_vertex.hpp>

namespace mbgl {
namespace gl {

namespace gl {
class Context;
} // namespace gl

class Mesh {
public:
    Mesh(Size size);

    void upload(gl::Context&);

    std::vector<FillVertex> vertices;
    std::vector<gl::Triangle> triangles;
    std::vector<gl::Segment> triangleSegments { { 0, 0 } };

    optional<gl::VertexBuffer<FillVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> triangleIndexBuffer;
};

} // namespace gl
} // namespace mbgl
