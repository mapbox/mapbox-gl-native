#ifndef MBGL_MAP_TILE_PARSER
#define MBGL_MAP_TILE_PARSER

#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/class_properties.hpp>
#include <mbgl/style/style_bucket.hpp>
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
class StyleLayoutFill;
class StyleLayoutRaster;
class StyleLayoutLine;
class StyleLayoutSymbol;
class StyleLayerGroup;
class VectorTileData;
class Collision;
class TexturePool;

class TileParser : private util::noncopyable
{
public:
    TileParser(const std::string& rawData,
               VectorTileData& tile,
               const util::ptr<const Style>& style,
               GlyphAtlas& glyphAtlas,
               GlyphStore& glyphStore,
               SpriteAtlas& spriteAtlas,
               const util::ptr<Sprite>& sprite);
    ~TileParser();

public:
    void parse();

private:
    bool obsolete() const;
    void parseStyleLayers(util::ptr<const StyleLayerGroup> group);

    std::unique_ptr<Bucket> createBucket(const StyleBucket& bucketDesc);
    std::unique_ptr<Bucket> createFillBucket(const GeometryTileLayer& layer, const StyleBucket& bucketDesc);
    std::unique_ptr<Bucket> createLineBucket(const GeometryTileLayer& layer, const StyleBucket& bucketDesc);
    std::unique_ptr<Bucket> createSymbolBucket(const GeometryTileLayer& layer, const StyleBucket& bucketDesc);

    template <class Bucket> void addBucketGeometries(Bucket& bucket, const GeometryTileLayer& layer, const FilterExpression& filter);

private:
    const VectorTile vectorTile;
    VectorTileData& tile;

    // Cross-thread shared data.
    util::ptr<const Style> style;
    GlyphAtlas& glyphAtlas;
    GlyphStore& glyphStore;
    SpriteAtlas& spriteAtlas;
    util::ptr<Sprite> sprite;

    std::unique_ptr<Collision> collision;
};

}

#endif
