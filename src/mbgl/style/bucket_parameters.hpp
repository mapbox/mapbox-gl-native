#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/filter.hpp>

#include <atomic>
#include <functional>

namespace mbgl {

class TileID;
class GeometryTileLayer;
class GeometryTileFeature;
class SpriteStore;
class GlyphAtlas;
class GlyphStore;
class CollisionTile;
class FeatureIndex;

namespace style {

class BucketParameters {
public:
    BucketParameters(const OverscaledTileID& tileID_,
                          const GeometryTileLayer& layer_,
                          const std::atomic<bool>& obsolete_,
                          uintptr_t tileUID_,
                          bool& partialParse_,
                          SpriteStore& spriteStore_,
                          GlyphAtlas& glyphAtlas_,
                          GlyphStore& glyphStore_,
                          FeatureIndex& featureIndex_,
                          const MapMode mode_)
        : tileID(tileID_),
          layer(layer_),
          obsolete(obsolete_),
          tileUID(tileUID_),
          partialParse(partialParse_),
          spriteStore(spriteStore_),
          glyphAtlas(glyphAtlas_),
          glyphStore(glyphStore_),
          featureIndex(featureIndex_),
          mode(mode_) {}

    bool cancelled() const {
        return obsolete;
    }

    void eachFilteredFeature(const Filter&, std::function<void (const GeometryTileFeature&, std::size_t index, const std::string& layerName)>);

    const OverscaledTileID& tileID;
    const GeometryTileLayer& layer;
    const std::atomic<bool>& obsolete;
    uintptr_t tileUID;
    bool& partialParse;
    SpriteStore& spriteStore;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    FeatureIndex& featureIndex;
    const MapMode mode;
};

} // namespace style
} // namespace mbgl
