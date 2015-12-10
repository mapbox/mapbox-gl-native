#ifndef MBGL_RENDERER_DEBUGBUCKET
#define MBGL_RENDERER_DEBUGBUCKET

#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/geometry/debug_font_buffer.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class PlainShader;

class DebugBucket final : private util::noncopyable {
public:
    DebugBucket(TileID id, TileData::State, Seconds modified, Seconds expires, MapDebugOptions);

    void drawLines(PlainShader& shader);
    void drawPoints(PlainShader& shader);

    const TileData::State state;
    const Seconds modified;
    const Seconds expires;
    const MapDebugOptions debugMode;

private:
    DebugFontBuffer fontBuffer;
    VertexArrayObject array;
};

} // namespace mbgl

#endif
