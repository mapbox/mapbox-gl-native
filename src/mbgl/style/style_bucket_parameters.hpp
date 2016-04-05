#ifndef STYLE_BUCKET_PARAMETERS
#define STYLE_BUCKET_PARAMETERS

#include <mbgl/map/mode.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/tile/tile_data.hpp>

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

class StyleBucketParameters {
public:
    StyleBucketParameters(const TileID& tileID_,
                          const GeometryTileLayer& layer_,
                          const std::atomic<TileData::State>& state_,
                          uintptr_t tileUID_,
                          bool& partialParse_,
                          SpriteStore& spriteStore_,
                          GlyphAtlas& glyphAtlas_,
                          GlyphStore& glyphStore_,
                          FeatureIndex& featureIndex_,
                          const MapMode mode_)
        : tileID(tileID_),
          layer(layer_),
          state(state_),
          tileUID(tileUID_),
          partialParse(partialParse_),
          spriteStore(spriteStore_),
          glyphAtlas(glyphAtlas_),
          glyphStore(glyphStore_),
          featureIndex(featureIndex_),
          mode(mode_) {}

    bool cancelled() const {
        return state == TileData::State::obsolete;
    }

    void eachFilteredFeature(const Filter&, std::function<void (const GeometryTileFeature&, std::size_t index, const std::string& layerName)>);

    const TileID& tileID;
    const GeometryTileLayer& layer;
    const std::atomic<TileData::State>& state;
    uintptr_t tileUID;
    bool& partialParse;
    SpriteStore& spriteStore;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    FeatureIndex& featureIndex;
    const MapMode mode;
};

} // namespace mbgl

#endif
