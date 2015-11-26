#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

#include <mbgl/map/tile_data.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>

namespace mbgl {

class PlainShader;

class DebugBucket : private util::noncopyable {
public:
    DebugBucket(TileID id, TileData::State);

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

    const TileData::State state;

private:
    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

}

#endif
