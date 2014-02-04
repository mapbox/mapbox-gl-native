#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/debug_font_buffer.hpp"
#include "../geometry/fill_buffer.hpp"
#include "../geometry/line_buffer.hpp"

#include "layer.hpp"

#include <stdint.h>
#include <forward_list>
#include <mutex>
#include <llmr/util/vec.hpp>
#include <string>
#include <map>
#include <memory>

namespace llmr {

struct pbf;
class Style;
class Bucket;
class LayerDescription;
class BucketDescription;
class VectorTile;
class VectorTileLayer;



class Tile : public std::enable_shared_from_this<Tile> {
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

    // Make noncopyable
    Tile(const Tile&) = delete;
    Tile(const Tile&&) = delete;
    Tile& operator=(const Tile&) = delete;
    Tile& operator=(const Tile && ) = delete;

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
    std::shared_ptr<DebugFontBuffer> debugFontBuffer;
    std::shared_ptr<FillBuffer> fillBuffer;
    std::shared_ptr<LineBuffer> lineBuffer;

    // Holds the buckets of this tile.
    // They contain the location offsets in the buffers stored above
    std::map<std::string, std::shared_ptr<Bucket>> buckets;
    // std::forward_list<Layer> layers;

private:
    // Source data
    std::string data;

    // std::mutex mtx;

    const Style& style;
};

}

#endif
