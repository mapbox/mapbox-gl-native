#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class OverscaledTileID;
class PlainShader;

namespace gl {
class ObjectStore;
class Context;
} // namespace gl

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(const OverscaledTileID& id,
                bool renderable,
                bool complete,
                optional<Timestamp> modified,
                optional<Timestamp> expires,
                MapDebugOptions);

    void drawLines(PlainShader&, gl::ObjectStore&, gl::Context&);
    void drawPoints(PlainShader&, gl::ObjectStore&, gl::Context&);

    const bool renderable;
    const bool complete;
    const optional<Timestamp> modified;
    const optional<Timestamp> expires;
    const MapDebugOptions debugMode;

private:
    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

} // namespace mbgl
