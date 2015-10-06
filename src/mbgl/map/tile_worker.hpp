#ifndef MBGL_MAP_TILE_WORKER
#define MBGL_MAP_TILE_WORKER

#include <mapbox/variant.hpp>

#include <mbgl/map/tile_data.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/style/filter_expression.hpp>

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace mbgl {

class CollisionTile;
class GeometryTile;
class Style;
class Bucket;
class StyleLayer;
class StyleBucket;
class GeometryTileLayer;

using TileParseResult = mapbox::util::variant<
    TileData::State, // success
    std::string>;    // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(TileID,
               std::string sourceID,
               Style&,
               std::vector<util::ptr<StyleLayer>>,
               const std::atomic<TileData::State>&,
               std::unique_ptr<CollisionTile>);
    ~TileWorker();

    Bucket* getBucket(const StyleLayer&) const;

    TileParseResult parse(const GeometryTile&);
    void redoPlacement(float angle, float pitch, bool collisionDebug);

    std::vector<util::ptr<StyleLayer>> layers;

private:
    void parseLayer(const StyleLayer&, const GeometryTile&);

    std::unique_ptr<Bucket> createFillBucket(const GeometryTileLayer&, const StyleBucket&);
    std::unique_ptr<Bucket> createLineBucket(const GeometryTileLayer&, const StyleBucket&);
    std::unique_ptr<Bucket> createCircleBucket(const GeometryTileLayer&, const StyleBucket&);
    std::unique_ptr<Bucket> createSymbolBucket(const GeometryTileLayer&, const StyleBucket&);

    template <class Bucket>
    void addBucketGeometries(Bucket&, const GeometryTileLayer&, const FilterExpression&);

    const TileID id;
    const std::string sourceID;

    Style& style;
    const std::atomic<TileData::State>& state;

    bool partialParse = false;

    std::unique_ptr<CollisionTile> collisionTile;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added to this map as they get processed.
    // Tiles partially parsed can get new buckets at any moment but are
    // also fit for rendering. That said, access to this list needs locking
    // unless the tile is completely parsed.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    mutable std::mutex bucketsMutex;
};

}

#endif
