#include <mbgl/map/tile_parser.hpp>

#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/collision.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/map/map.hpp>

#include <mbgl/util/std.hpp>
#include <mbgl/util/utf.hpp>

#include <locale>

#ifdef __linux__
#include <boost/regex.hpp>
namespace regex_impl = boost;
#else
#include <regex>
namespace regex_impl = std;
#endif

namespace mbgl {

// Note: This destructor is seemingly empty, but we need to declare it anyway
// because this object has a std::unique_ptr<> of a forward-declare type in
// its header file.
TileParser::~TileParser() = default;

TileParser::TileParser(const std::string &data, VectorTileData &tile,
                       const std::shared_ptr<const Style> &style,
                       const std::shared_ptr<GlyphAtlas> &glyphAtlas,
                       const std::shared_ptr<GlyphStore> &glyphStore,
                       const std::shared_ptr<SpriteAtlas> &spriteAtlas,
                       const std::shared_ptr<Sprite> &sprite)
    : vector_data(pbf((const uint8_t *)data.data(), data.size())),
      tile(tile),
      style(style),
      glyphAtlas(glyphAtlas),
      glyphStore(glyphStore),
      spriteAtlas(spriteAtlas),
      sprite(sprite),
      collision(std::make_unique<Collision>(tile.id.z, 4096, tile.source.tile_size, tile.depth)) {
}

void TileParser::parse() {
    parseStyleLayers(style->layers);
}

bool TileParser::obsolete() const { return tile.state == TileData::State::obsolete; }

void TileParser::parseStyleLayers(std::shared_ptr<StyleLayerGroup> group) {
    if (!group) {
        return;
    }

    for (const std::shared_ptr<StyleLayer> &layer_desc : group->layers) {
        // Cancel early when parsing.
        if (obsolete()) {
            return;
        }

        if (layer_desc->isBackground()) {
            // background is a special, fake bucket
            continue;
        } else if (layer_desc->layers) {
            // This is a layer group.
            parseStyleLayers(layer_desc->layers);
        }
        if (layer_desc->bucket) {
            // This is a singular layer. Check if this bucket already exists. If not,
            // parse this bucket.
            auto bucket_it = tile.buckets.find(layer_desc->bucket->name);
            if (bucket_it == tile.buckets.end()) {
                // We need to create this bucket since it doesn't exist yet.
                std::unique_ptr<Bucket> bucket = createBucket(layer_desc->bucket);
                if (bucket) {
                    // Bucket creation might fail because the data tile may not
                    // contain any data that falls into this bucket.
                    tile.buckets[layer_desc->bucket->name] = std::move(bucket);
                }
            }
        } else {
            fprintf(stderr, "[WARNING] layer '%s' does not have child layers or buckets\n", layer_desc->id.c_str());
        }
    }
}

std::unique_ptr<Bucket> TileParser::createBucket(std::shared_ptr<StyleBucket> bucket_desc) {
    if (!bucket_desc) {
        fprintf(stderr, "missing bucket desc\n");
        return nullptr;
    }

    // Skip this bucket if we are to not render this
    if (tile.id.z < std::floor(bucket_desc->min_zoom) && std::floor(bucket_desc->min_zoom) < tile.source.max_zoom) return nullptr;
    if (tile.id.z >= std::ceil(bucket_desc->max_zoom)) return nullptr;

    auto layer_it = vector_data.layers.find(bucket_desc->source_layer);
    if (layer_it != vector_data.layers.end()) {
        const VectorTileLayer &layer = layer_it->second;
        if (bucket_desc->render.is<StyleBucketFill>()) {
            return createFillBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketFill>());
        } else if (bucket_desc->render.is<StyleBucketLine>()) {
            return createLineBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketLine>());
        } else if (bucket_desc->render.is<StyleBucketSymbol>()) {
            return createSymbolBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketSymbol>());
        } else if (bucket_desc->render.is<StyleBucketRaster>()) {
            return nullptr;
        } else {
            fprintf(stderr, "[WARNING] unknown bucket render type for layer '%s' (source layer '%s')\n", bucket_desc->name.c_str(), bucket_desc->source_layer.c_str());
        }
    } else if (bucket_desc->render.is<StyleBucketRaster>() && bucket_desc->render.get<StyleBucketRaster>().prerendered == true) {
        return createRasterBucket(texturePool, bucket_desc->render.get<StyleBucketRaster>());
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
        if (debug::tileParseWarnings) {
            fprintf(stderr, "[WARNING] layer '%s' does not exist in tile %d/%d/%d\n",
                    bucket_desc->source_layer.c_str(), tile.id.z, tile.id.x, tile.id.y);
        }
    }

    return nullptr;
}

template <class Bucket>
void TileParser::addBucketGeometries(Bucket& bucket, const VectorTileLayer& layer, const FilterExpression &filter) {
    FilteredVectorTileLayer filtered_layer(layer, filter);
    for (pbf feature : filtered_layer) {
        if (obsolete())
            return;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            } else if (debug::tileParseWarnings) {
                fprintf(stderr, "[WARNING] geometry is empty\n");
            }
        }
    }
}

std::unique_ptr<Bucket> TileParser::createFillBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketFill &fill) {
    std::unique_ptr<FillBucket> bucket = std::make_unique<FillBucket>(tile.fillVertexBuffer, tile.triangleElementsBuffer, tile.lineElementsBuffer, fill);
    addBucketGeometries(bucket, layer, filter);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createRasterBucket(const std::shared_ptr<Texturepool> &texturepool, const StyleBucketRaster &raster) {
    std::unique_ptr<RasterBucket> bucket = std::make_unique<RasterBucket>(texturepool, raster);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createLineBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketLine &line) {
    std::unique_ptr<LineBucket> bucket = std::make_unique<LineBucket>(tile.lineVertexBuffer, tile.triangleElementsBuffer, tile.pointElementsBuffer, line);
    addBucketGeometries(bucket, layer, filter);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createSymbolBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketSymbol &symbol) {
    std::unique_ptr<SymbolBucket> bucket = std::make_unique<SymbolBucket>(symbol, *collision);
    bucket->addFeatures(layer, filter, tile.id, *spriteAtlas, *sprite, *glyphAtlas, *glyphStore);
    return obsolete() ? nullptr : std::move(bucket);
}

}
