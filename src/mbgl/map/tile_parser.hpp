#ifndef MBGL_MAP_TILE_PARSER
#define MBGL_MAP_TILE_PARSER

#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace mbgl {

class Bucket;
class TexturePool;
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
class TexturePool;

class TileParser : private util::noncopyable
{
public:
    TileParser(const std::string &data, VectorTileData &tile,
               const util::ptr<const Style> &style,
               GlyphAtlas & glyphAtlas,
               GlyphStore & glyphStore,
               SpriteAtlas & spriteAtlas,
               const util::ptr<Sprite> &sprite,
               TexturePool& texturePool);
    ~TileParser();

public:
    void parse();

private:
    bool obsolete() const;
    void parseStyleLayers(util::ptr<StyleLayerGroup> group);
    std::unique_ptr<Bucket> createBucket(util::ptr<StyleBucket> bucket_desc);

    std::unique_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketFill &fill);
    std::unique_ptr<Bucket> createRasterBucket(const StyleBucketRaster &raster);
    std::unique_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketLine &line);
    std::unique_ptr<Bucket> createSymbolBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketSymbol &symbol);

    template <class Bucket> void addBucketGeometries(Bucket& bucket, const VectorTileLayer& layer, const FilterExpression &filter);

private:
    const VectorTile vector_data;
    VectorTileData& tile;

    // Cross-thread shared data.
    util::ptr<const Style> style;
    GlyphAtlas & glyphAtlas;
    GlyphStore & glyphStore;
    SpriteAtlas & spriteAtlas;
    util::ptr<Sprite> sprite;
    TexturePool& texturePool;

    std::unique_ptr<Collision> collision;
};

}

#endif
