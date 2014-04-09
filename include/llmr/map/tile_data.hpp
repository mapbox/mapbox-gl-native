#ifndef LLMR_MAP_TILE_DATA
#define LLMR_MAP_TILE_DATA

#include <llmr/map/tile.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/platform/platform.hpp>

#include <llmr/renderer/bucket.hpp>

#include <llmr/geometry/vertex_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>
#include <llmr/geometry/text_buffer.hpp>

#include <cstdint>
#include <forward_list>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>

namespace uv {
class loop;
}

namespace llmr {

class Style;
class Raster;
class LayerDescription;
class BucketDescription;


class PlainShader;

class TileData : public std::enable_shared_from_this<TileData>,
             private util::noncopyable {
public:
    struct exception : std::exception {};
    struct geometry_too_long_exception : exception {};

public:
    typedef std::shared_ptr<TileData> Ptr;

    enum class State {
        invalid,
        initial,
        loading,
        loaded,
        parsed,
        obsolete
    };

public:
    TileData(Tile::ID id, uv::loop &loop, const Style& style, GlyphAtlas& glyphAtlas, const bool use_raster = false, const bool use_retina = false);
    ~TileData();

    void request(std::function<void()> callback);
    void reparse(std::function<void()> callback);
    void parse();
    void cancel();

    const std::string toString() const;

public:
    const Tile::ID id;
    const bool use_raster;
    const bool use_retina;
    std::atomic<State> state;
    std::shared_ptr<Raster> raster;

    // Holds the actual geometries in this tile.
    DebugFontBuffer debugFontBuffer;
    VertexArrayObject debugFontArray;

    FillVertexBuffer fillVertexBuffer;
    LineVertexBuffer lineVertexBuffer;
    PointVertexBuffer pointVertexBuffer;
    TextVertexBuffer textVertexBuffer;

    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;
    PointElementsBuffer pointElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::map<std::string, std::unique_ptr<Bucket>> buckets;

private:
    // Event loop
    uv::loop &loop;

    // Source data
    std::string data;
    const Style& style;
    GlyphAtlas& glyphAtlas;

    // Stores a request that is in progress.
    std::weak_ptr<platform::Request> req;
};

}

#endif
