#ifndef LLMR_MAP_TILE_PARSER
#define LLMR_MAP_TILE_PARSER

#include <llmr/map/tile_data.hpp>
#include <llmr/text/placement.hpp>

namespace llmr {

class Style;
class GlyphAtlas;

class TileParser {
public:
    TileParser(const std::string& data, TileData& tile, const Style& style, GlyphAtlas& glyphAtlas, bool is_raster = false);

private:
    bool obsolete() const;
    void parseGlyphs();
    void parseStyleLayers(const std::vector<LayerDescription>& layers);
    std::unique_ptr<Bucket> createBucket(const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createPointBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createTextBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc);
    std::unique_ptr<Bucket> createRasterBucket(const BucketDescription& bucket_desc);
    template <class Bucket> void addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc);

private:
    const VectorTile vector_data;
    const std::string raster_data;
    TileData& tile;
    const Style& style;
    GlyphAtlas& glyphAtlas;
    Faces faces;
    Placement placement;
};

}

#endif
