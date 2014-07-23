#ifndef MBGL_MAP_TILE_PARSER
#define MBGL_MAP_TILE_PARSER

#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/placement.hpp>

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

namespace mbgl {

class Bucket;
class FontStack;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class Style;
class StyleBucket;
class StyleBucketFill;
class StyleBucketIcon;
class StyleBucketLine;
class StyleBucketText;
class StyleLayerGroup;
class VectorTileData;

class TileParser {
public:
    TileParser(const std::string &data, VectorTileData &tile,
               const std::shared_ptr<const Style> &style,
               const std::shared_ptr<GlyphAtlas> &glyphAtlas,
               const std::shared_ptr<GlyphStore> &glyphStore,
               const std::shared_ptr<SpriteAtlas> &spriteAtlas);

public:
    void parse();

private:
    bool obsolete() const;
    void parseGlyphs();
    void parseStyleLayers(std::shared_ptr<StyleLayerGroup> group);
    void addGlyph(uint64_t tileid, const std::string stackname, const std::u32string &string, const FontStack &fontStack, GlyphAtlas &glyphAtlas, GlyphPositions &face);
    std::unique_ptr<Bucket> createBucket(std::shared_ptr<StyleBucket> bucket_desc);

    std::unique_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketFill &fill);
    std::unique_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketLine &line);
    std::unique_ptr<Bucket> createIconBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketIcon &icon);
    std::unique_ptr<Bucket> createTextBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketText &text);

    template <class Bucket> void addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const FilterExpression &filter);
    template <class Bucket, typename ...Args> void addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const FilterExpression &filter, Args&& ...args);

private:
    const VectorTile vector_data;
    VectorTileData& tile;

    // Cross-thread shared data.
    std::shared_ptr<const Style> style;
    std::shared_ptr<GlyphAtlas> glyphAtlas;
    std::shared_ptr<GlyphStore> glyphStore;
    std::shared_ptr<SpriteAtlas> spriteAtlas;

    Placement placement;
};

}

#endif
