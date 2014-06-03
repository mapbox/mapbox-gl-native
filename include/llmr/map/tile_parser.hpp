#ifndef LLMR_MAP_TILE_PARSER
#define LLMR_MAP_TILE_PARSER

#include <llmr/map/vector_tile.hpp>
#include <llmr/text/placement.hpp>
#include <llmr/text/glyph_store.hpp>
#include <llmr/text/glyph.hpp>
#include <llmr/util/utf.hpp>

namespace llmr {

class Style;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class VectorTileData;

class LayerDescription;
class Bucket;

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
    void parseStyleLayers(const std::vector<LayerDescription>& layers);
    void addGlyph(uint64_t tileid, const std::string stackname, const std::u32string &string, const FontStack &fontStack, GlyphAtlas &glyphAtlas, GlyphPositions &face);
    std::unique_ptr<Bucket> createBucket(const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createIconBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createTextBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    template <class Bucket> void addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    template <class Bucket, typename ...Args> void addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc, Args&& ...args);

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
