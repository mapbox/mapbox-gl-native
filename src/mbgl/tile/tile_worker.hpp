#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/geometry/feature_index.hpp>

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
class TileParseResultData {
public:
    bool complete = false;
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTile> geometryTile;
};

using TileParseResult = variant<
    TileParseResultData, // success
    std::exception_ptr>;    // error

class TileWorker : public util::noncopyable {
public:
    TileWorker(const OverscaledTileID&,
               std::string sourceID,
               SpriteStore&,
               GlyphAtlas&,
               GlyphStore&,
               const std::atomic<bool>&,
               const MapMode);
    ~TileWorker();

    TileParseResult parseAllLayers(std::vector<std::unique_ptr<StyleLayer>>,
                                   std::unique_ptr<const GeometryTile> geometryTile,
                                   PlacementConfig);

    TileParseResult parsePendingLayers(PlacementConfig);

    std::unique_ptr<CollisionTile> redoPlacement(const std::unordered_map<std::string, std::unique_ptr<Bucket>>*,
                       PlacementConfig);

private:
    TileParseResult prepareResult(const PlacementConfig& config);
    void parseLayer(const StyleLayer*);
    void insertBucket(const std::string& name, std::unique_ptr<Bucket>);
    std::unique_ptr<CollisionTile> placeLayers(PlacementConfig);

    const OverscaledTileID id;
    const std::string sourceID;

    SpriteStore& spriteStore;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    const std::atomic<bool>& obsolete;
    const MapMode mode;

    bool partialParse = false;

    std::vector<std::unique_ptr<StyleLayer>> layers;

    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTile> geometryTile;

    // Contains buckets that we couldn't parse so far due to missing resources.
    // They will be attempted on subsequent parses.
    std::list<std::pair<const SymbolLayer*, std::unique_ptr<Bucket>>> pending;

    // Contains buckets that have been parsed, but still need placement.
    // They will be placed when all buckets have been parsed.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> placementPending;

    // Temporary holder
    TileParseResultData result;
};

} // namespace mbgl
