#ifndef STYLE_BUCKET_PARAMETERS
#define STYLE_BUCKET_PARAMETERS

#include <mbgl/style/filter_expression.hpp>
#include <mbgl/map/tile_data.hpp>

#include <functional>

namespace mbgl {

class TileID;
class GeometryTileLayer;
class GeometryTileFeature;
class SpriteAtlas;
class Sprite;
class GlyphAtlas;
class GlyphStore;
class CollisionTile;

class StyleBucketParameters {
public:
    StyleBucketParameters(const TileID& tileID_,
                          const GeometryTileLayer& layer_,
                          const std::atomic<TileData::State>& state_,
                          uintptr_t tileUID_,
                          bool& partialParse_,
                          SpriteAtlas& spriteAtlas_,
                          Sprite& sprite_,
                          GlyphAtlas& glyphAtlas_,
                          GlyphStore& glyphStore_,
                          CollisionTile& collisionTile_)
        : tileID(tileID_),
          layer(layer_),
          state(state_),
          tileUID(tileUID_),
          partialParse(partialParse_),
          spriteAtlas(spriteAtlas_),
          sprite(sprite_),
          glyphAtlas(glyphAtlas_),
          glyphStore(glyphStore_),
          collisionTile(collisionTile_) {}

    bool cancelled() const {
        return state == TileData::State::obsolete;
    }

    void eachFilteredFeature(const FilterExpression&, std::function<void (const GeometryTileFeature&)>);

    const TileID& tileID;
    const GeometryTileLayer& layer;
    const std::atomic<TileData::State>& state;
    uintptr_t tileUID;
    bool& partialParse;
    SpriteAtlas& spriteAtlas;
    Sprite& sprite;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    CollisionTile& collisionTile;
};

}

#endif
