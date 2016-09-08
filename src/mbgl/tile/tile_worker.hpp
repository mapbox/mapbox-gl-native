#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <atomic>
#include <string>
#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>

namespace mbgl {

class CollisionTile;
class GeometryTileData;
class SpriteStore;
class GlyphAtlas;
class GlyphStore;
class Bucket;
class SymbolLayout;
class PlacementConfig;

namespace style {
class Layer;
} // namespace style

// We're using this class to shuttle the resulting buckets from the worker thread to the MapContext
// thread. This class is movable-only because the vector contains movable-only value elements.
class TileParseResultData {
public:
    bool complete = false;
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTileData> tileData;
};

using TileParseResult = variant<
    TileParseResultData, // success
    std::exception_ptr>;    // error

class TilePlacementResult {
public:
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    std::unique_ptr<CollisionTile> collisionTile;
};

class TileWorker : public util::noncopyable {
public:
    TileWorker(OverscaledTileID,
               SpriteStore&,
               GlyphAtlas&,
               GlyphStore&,
               const std::atomic<bool>&,
               const MapMode);
    ~TileWorker();

    TileParseResult parseAllLayers(std::vector<std::unique_ptr<style::Layer>>,
                                   std::unique_ptr<const GeometryTileData>,
                                   const PlacementConfig&);

    TileParseResult parsePendingLayers(const PlacementConfig&);

    TileParseResult redoLayout(std::vector<std::unique_ptr<style::Layer>>,
                               const PlacementConfig&);

    TilePlacementResult redoPlacement(const PlacementConfig&);

private:
    TileParseResult parsePendingLayers(const PlacementConfig&,
                                       std::unordered_map<std::string, std::unique_ptr<Bucket>>);

    const OverscaledTileID id;

    SpriteStore& spriteStore;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    const std::atomic<bool>& obsolete;
    const MapMode mode;

    std::vector<std::unique_ptr<style::Layer>> layers;
    std::unique_ptr<const GeometryTileData> tileData;

    std::unique_ptr<FeatureIndex> featureIndex;
    std::vector<std::unique_ptr<SymbolLayout>> symbolLayouts;
};

} // namespace mbgl
