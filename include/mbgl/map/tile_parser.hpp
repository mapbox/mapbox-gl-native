#ifndef MBGL_MAP_TILE_PARSER
#define MBGL_MAP_TILE_PARSER

#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <iosfwd>
#include <string>

namespace mbgl {

class Bucket;
class Texturepool;
class FontStack;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class Sprite;
class Style;
class StyleBucket;
class StyleBucketFill;
class StyleBucketRaster;
class StyleBucketLine;
class StyleBucketSymbol;
class StyleLayerGroup;
class VectorTileData;
class Collision;

class TileParser {
public:
    TileParser(const std::string &data, VectorTileData &tile,
               const util::ptr<const Style> &style,
               const util::ptr<GlyphAtlas> &glyphAtlas,
               const util::ptr<GlyphStore> &glyphStore,
               const util::ptr<SpriteAtlas> &spriteAtlas,
               const util::ptr<Sprite> &sprite);
    ~TileParser();

public:
    void parse();

private:
    bool obsolete() const;
    void parseStyleLayers(util::ptr<StyleLayerGroup> group);
    std::unique_ptr<Bucket> createBucket(util::ptr<StyleBucket> bucket_desc);

    std::unique_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketFill &fill);
    std::unique_ptr<Bucket> createRasterBucket(const util::ptr<Texturepool> &texturepool, const StyleBucketRaster &raster);
    std::unique_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketLine &line);
    std::unique_ptr<Bucket> createSymbolBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketSymbol &symbol);

    template <class Bucket> void addBucketGeometries(Bucket& bucket, const VectorTileLayer& layer, const FilterExpression &filter);

private:
    const VectorTile vector_data;
    VectorTileData& tile;

    // Cross-thread shared data.
    util::ptr<const Style> style;
    util::ptr<GlyphAtlas> glyphAtlas;
    util::ptr<GlyphStore> glyphStore;
    util::ptr<SpriteAtlas> spriteAtlas;
    util::ptr<Sprite> sprite;
    util::ptr<Texturepool> texturePool;

    std::unique_ptr<Collision> collision;
};

}

#endif
