#ifndef MBGL_MAP_TILE_WORKER
#define MBGL_MAP_TILE_WORKER

#include <mapbox/variant.hpp>

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_data.hpp>
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
class SpriteStore;
class GlyphAtlas;
class GlyphStore;
class Bucket;
class StyleLayer;
class SymbolLayer;

// We're using this class to shuttle the resulting buckets from the worker thread to the MapContext
// thread. This class is movable-only because the vector contains movable-only value elements.
class TileParseResultBuckets {
public:
    TileData::State state = TileData::State::invalid;
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
};

using TileParseResult = mapbox::util::variant<
    TileParseResultBuckets, // success
    std::exception_ptr>;    // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(TileID,
               std::string sourceID,
               SpriteStore&,
               GlyphAtlas&,
               GlyphStore&,
               const std::atomic<TileData::State>&,
               const MapMode);
    ~TileWorker();

    TileParseResult parseAllLayers(std::vector<std::unique_ptr<StyleLayer>>,
                                   std::unique_ptr<const GeometryTile> geometryTile,
                                   PlacementConfig);

    TileParseResult parsePendingLayers(PlacementConfig);

    void redoPlacement(const std::unordered_map<std::string, std::unique_ptr<Bucket>>*,
                       PlacementConfig);

private:
    void parseLayer(const StyleLayer*, const GeometryTile&);
    void insertBucket(const std::string& name, std::unique_ptr<Bucket>);
    void placeLayers(PlacementConfig);

    const TileID id;
    const std::string sourceID;

    SpriteStore& spriteStore;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    const std::atomic<TileData::State>& state;
    const MapMode mode;

    bool partialParse = false;

    std::vector<std::unique_ptr<StyleLayer>> layers;

    // Contains buckets that we couldn't parse so far due to missing resources.
    // They will be attempted on subsequent parses.
    std::list<std::pair<const SymbolLayer*, std::unique_ptr<Bucket>>> pending;

    // Contains buckets that have been parsed, but still need placement.
    // They will be placed when all buckets have been parsed.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> placementPending;

    // Temporary holder
    TileParseResultBuckets result;
};

} // namespace mbgl

#endif
