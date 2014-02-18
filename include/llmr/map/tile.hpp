#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include <llmr/util/vec.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/geometry/debug_font_buffer.hpp>
#include <llmr/geometry/vao.hpp>

#include <cstdint>
#include <forward_list>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace llmr {

class Style;
class Bucket;
class LayerDescription;
class BucketDescription;
class VectorTile;
class VectorTileLayer;
class FillVertexBuffer;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineElementsBuffer;
class PointElementsBuffer;
class PlainShader;

class Tile : public std::enable_shared_from_this<Tile>,
             private util::noncopyable {
public:
    struct exception : std::exception {};
    struct geometry_too_long_exception : exception {};

public:
    typedef std::shared_ptr<Tile> Ptr;

    typedef vec3<int32_t> ID;

    enum state {
        initial,
        loading,
        ready,
        obsolete
    };

public:
    Tile(ID id, const Style& style);
    ~Tile();

    // Start loading the tile.
    void request();

    // Other functions
    bool parse();
    void parseStyleLayers(const VectorTile& tile, const std::vector<LayerDescription>& layers);
    std::shared_ptr<Bucket> createBucket(const VectorTile& tile, const BucketDescription& bucket_desc);
    std::shared_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::shared_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);

    void cancel();

    const std::string toString() const;

    static ID parent(const ID& id, int32_t z);
    static std::forward_list<ID> children(const ID& id, int32_t z);

public:
    const ID id;
    state state;

    // Holds the actual geometries in this tile.
    DebugFontBuffer debugFontBuffer;
    VertexArrayObject debugFontArray;

    std::shared_ptr<FillVertexBuffer> fillVertexBuffer;
    std::shared_ptr<LineVertexBuffer> lineVertexBuffer;

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
};

}

#endif
