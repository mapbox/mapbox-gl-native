#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

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
    DebugBucket(TileID id, TileData::State,
                optional<SystemTimePoint> modified,
                optional<SystemTimePoint> expires,
                MapDebugOptions);

    void drawLines(PlainShader&, gl::GLObjectStore&);
    void drawPoints(PlainShader&, gl::GLObjectStore&);

    const TileData::State state;
    const optional<SystemTimePoint> modified;
    const optional<SystemTimePoint> expires;
    const MapDebugOptions debugMode;

private:
    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

} // namespace mbgl

#endif
