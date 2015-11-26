#ifndef MBGL_MAP_TILE_WORKER
#define MBGL_MAP_TILE_WORKER

#include <mapbox/variant.hpp>

#include <mbgl/map/tile_data.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
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

// We're using this class to shuttle the resulting buckets from the worker thread to the MapContext
// thread. This class is movable-only because the vector contains movable-only value elements.
class TileParseResultBuckets {
public:
    TileData::State state = TileData::State::invalid;
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
};

using TileParseResult = mapbox::util::variant<TileParseResultBuckets, // success
                                              std::string>;           // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(TileID,
               std::string sourceID,
               Style&,
               const std::atomic<TileData::State>&);
    ~TileWorker();

    TileParseResult parseAllLayers(std::vector<util::ptr<StyleLayer>>,
                                   const GeometryTile&,
                                   PlacementConfig);

    TileParseResult parsePendingLayers();

    void redoPlacement(std::vector<util::ptr<StyleLayer>>,
                       const std::unordered_map<std::string, std::unique_ptr<Bucket>>*,
                       PlacementConfig);

private:
    void parseLayer(const StyleLayer&, const GeometryTile&);
    void insertBucket(const std::string& name, std::unique_ptr<Bucket>);

    const TileID id;
    const std::string sourceID;

    Style& style;
    const std::atomic<TileData::State>& state;

    bool partialParse = false;

    std::unique_ptr<CollisionTile> collisionTile;

    // Contains buckets that we couldn't parse so far due to missing resources.
    // They will be attempted on subsequent parses.
    std::list<std::pair<const StyleLayer&, std::unique_ptr<Bucket>>> pending;

    // Temporary holder
    TileParseResultBuckets result;
};

} // namespace mbgl

#endif
