#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/vao.hpp>
#include <mbgl/shader/fill_vertex.hpp>

namespace mbgl {

class OverscaledTileID;
class FillShader;

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

    void drawLines(FillShader&, gl::Context&);
    void drawPoints(FillShader&, gl::Context&);

    const bool renderable;
    const bool complete;
    const optional<Timestamp> modified;
    const optional<Timestamp> expires;
    const MapDebugOptions debugMode;

private:
    gl::VertexBuffer<FillVertex> vertexBuffer;
    gl::VertexArrayObject array;
};

} // namespace mbgl
