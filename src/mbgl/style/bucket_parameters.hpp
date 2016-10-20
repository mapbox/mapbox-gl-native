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
class GlyphAtlas;
class CollisionTile;
class FeatureIndex;

namespace style {

class BucketParameters {
public:
    BucketParameters(const OverscaledTileID& tileID_,
                          const std::atomic<bool>& obsolete_,
                          uintptr_t tileUID_,
                          GlyphAtlas& glyphAtlas_,
                          FeatureIndex& featureIndex_,
                          const MapMode mode_)
        : tileID(tileID_),
          obsolete(obsolete_),
          tileUID(tileUID_),
          glyphAtlas(glyphAtlas_),
          featureIndex(featureIndex_),
          mode(mode_) {}

    bool cancelled() const {
        return obsolete;
    }

    void eachFilteredFeature(const Filter&, const GeometryTileLayer&, std::function<void (const GeometryTileFeature&, std::size_t index, const std::string& layerName)>);

    const OverscaledTileID& tileID;
    const std::atomic<bool>& obsolete;
    uintptr_t tileUID;
    GlyphAtlas& glyphAtlas;
    FeatureIndex& featureIndex;
    const MapMode mode;
};

} // namespace style
} // namespace mbgl
