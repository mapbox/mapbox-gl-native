#ifndef LLMR_MAP_TILE_DATA
#define LLMR_MAP_TILE_DATA

#include <llmr/map/tile.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/util/mat4.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/platform/platform.hpp>

#include <cstdint>
#include <forward_list>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>

namespace llmr {

class Style;
class Raster;
class Bucket;
class LayerDescription;
class BucketDescription;
class VectorTile;
class VectorTileLayer;
class FillVertexBuffer;
class LineVertexBuffer;
class PointVertexBuffer;
class TriangleElementsBuffer;
class LineElementsBuffer;
class PointElementsBuffer;
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
    TileData(Tile::ID id, const Style& style, const bool use_raster = false, const bool use_retina = false);
    ~TileData();

    // Start loading the tile.
    void request();

    // Other functions
    bool parse();
    void parseStyleLayers(const VectorTile& tile, const std::vector<LayerDescription>& layers);
    std::shared_ptr<Bucket> createBucket(const VectorTile& tile, const BucketDescription& bucket_desc);
    std::shared_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::shared_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::shared_ptr<Bucket> createPointBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);

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

    std::shared_ptr<FillVertexBuffer> fillVertexBuffer;
    std::shared_ptr<LineVertexBuffer> lineVertexBuffer;
    std::shared_ptr<PointVertexBuffer> pointVertexBuffer;

    std::shared_ptr<TriangleElementsBuffer> triangleElementsBuffer;
    std::shared_ptr<LineElementsBuffer> lineElementsBuffer;
    std::shared_ptr<PointElementsBuffer> pointElementsBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::map<std::string, std::shared_ptr<Bucket>> buckets;
private:
    // Source data
    std::string data;
    const Style& style;
    platform::Request *req = nullptr;
};

}

#endif
