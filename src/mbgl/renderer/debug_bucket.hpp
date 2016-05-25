#pragma once

#include <mbgl/tile/tile_data.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class PlainShader;

namespace gl {
class GLObjectStore;
}

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(const OverscaledTileID& id,
                bool renderable,
                bool complete,
                optional<Timestamp> modified,
                optional<Timestamp> expires,
                MapDebugOptions);

    void drawLines(PlainShader&, gl::GLObjectStore&);
    void drawPoints(PlainShader&, gl::GLObjectStore&);

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
