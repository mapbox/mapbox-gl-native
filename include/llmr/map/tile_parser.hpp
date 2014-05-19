#ifndef LLMR_MAP_TILE_PARSER
#define LLMR_MAP_TILE_PARSER

#include <llmr/map/vector_tile_data.hpp>
#include <llmr/map/vector_tile.hpp>
#include <llmr/text/placement.hpp>

namespace llmr {

class Style;
class GlyphAtlas;
class SpriteAtlas;

class LayerDescription;
class Bucket;

class TileParser {
public:
    TileParser(const std::string& data, VectorTileData& tile, const Style& style, GlyphAtlas& glyphAtlas, SpriteAtlas &spriteAtlas);

private:
    bool obsolete() const;
    void parseGlyphs();
    void parseStyleLayers(const std::vector<LayerDescription>& layers);
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
    const Style& style;
    GlyphAtlas& glyphAtlas;
    SpriteAtlas &spriteAtlas;
    Faces faces;
    Placement placement;
};

}

#endif
