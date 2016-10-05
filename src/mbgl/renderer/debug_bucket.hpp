#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/vao.hpp>
#include <mbgl/shader/plain_vertex.hpp>

namespace mbgl {

class OverscaledTileID;
class PlainShader;

namespace gl {
class Context;
} // namespace gl

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(const OverscaledTileID& id,
                bool renderable,
                bool complete,
                optional<Timestamp> modified,
                optional<Timestamp> expires,
                MapDebugOptions,
                gl::Context&);

    void drawLines(PlainShader&, gl::Context&);
    void drawPoints(PlainShader&, gl::Context&);

    const bool renderable;
    const bool complete;
    const optional<Timestamp> modified;
    const optional<Timestamp> expires;
    const MapDebugOptions debugMode;

private:
    gl::VertexBuffer<PlainVertex> vertexBuffer;
    gl::VertexArrayObject array;
};

} // namespace mbgl
