#ifndef MBGL_MAP_TILE_WORKER
#define MBGL_MAP_TILE_WORKER

#include <mapbox/variant.hpp>

#include <mbgl/map/tile_data.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>
#include <mbgl/text/placement_config.hpp>

#include <string>
#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>

namespace mbgl {

class CollisionTile;
class GeometryTile;
class Style;
class Bucket;
class StyleLayer;
class StyleBucket;
class GeometryTileLayer;

// We're using this class to shuttle the resulting buckets from the worker thread to the MapContext
// thread. This class is movable-only because the vector contains movable-only value elements.
class TileParseResultBuckets {
public:
    TileData::State state = TileData::State::invalid;
    std::vector<std::pair<std::string, std::unique_ptr<Bucket>>> buckets;
};

using TileParseResult = mapbox::util::variant<TileParseResultBuckets, // success
                                              std::string>;           // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(TileID,
               std::string sourceID,
               Style&,
               std::vector<util::ptr<StyleLayer>>,
               const std::atomic<TileData::State>&);
    ~TileWorker();

    TileParseResult parseAllLayers(const GeometryTile&, PlacementConfig);
    TileParseResult parsePendingLayers();
    void redoPlacement(const std::unordered_map<std::string, std::unique_ptr<Bucket>>*,
                       PlacementConfig);

    std::vector<util::ptr<StyleLayer>> layers;

private:
    void parseLayer(const StyleLayer&, const GeometryTile&);

    void createFillBucket(const GeometryTileLayer&, const StyleBucket&);
    void createLineBucket(const GeometryTileLayer&, const StyleBucket&);
    void createCircleBucket(const GeometryTileLayer&, const StyleBucket&);
    void createSymbolBucket(const GeometryTileLayer&, const StyleBucket&);

    void insertBucket(const std::string& name, std::unique_ptr<Bucket>);

    template <class Bucket>
    void addBucketGeometries(Bucket&, const GeometryTileLayer&, const FilterExpression&);

    const TileID id;
    const std::string sourceID;
    const StyleCalculationParameters parameters;

    Style& style;
    const std::atomic<TileData::State>& state;

    std::unique_ptr<CollisionTile> collisionTile;

    // Contains buckets that we couldn't parse so far due to missing resources.
    // They will be attempted on subsequent parses.
    std::list<std::pair<const StyleBucket&, std::unique_ptr<Bucket>>> pending;

    // Temporary holder
    TileParseResultBuckets result;
};

} // namespace mbgl

#endif
