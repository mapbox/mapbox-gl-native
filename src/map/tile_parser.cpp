#include <llmr/map/tile_parser.hpp>

#include <llmr/style/style.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/geometry/glyph_atlas.hpp>

#include <llmr/util/std.hpp>

using namespace llmr;


TileParser::TileParser(const std::string& data, VectorTileData& tile, const Style& style, GlyphAtlas& glyphAtlas, SpriteAtlas &spriteAtlas)
    : vector_data(pbf((const uint8_t *)data.data(), data.size())),
      tile(tile),
      style(style),
      glyphAtlas(glyphAtlas),
      spriteAtlas(spriteAtlas),
      placement(tile.id.z) {
    parseGlyphs();
    parseStyleLayers(style.layers);
}

bool TileParser::obsolete() const {
    return tile.state == TileData::State::obsolete;
}

void TileParser::parseGlyphs() {
    for (const std::pair<std::string, const VectorTileFace> pair : vector_data.faces) {
        const std::string &name = pair.first;
        const VectorTileFace &face = pair.second;

        GlyphPositions &glyphs = faces[name];
        for (const VectorTileGlyph &glyph : face.glyphs) {
            const Rect<uint16_t> rect =
                glyphAtlas.addGlyph(tile.id.to_uint64(), name, glyph);
            glyphs.emplace(glyph.id, Glyph{rect, glyph.metrics});
        }
    }
}

void TileParser::parseStyleLayers(const std::vector<LayerDescription>& layers) {
    for (const LayerDescription& layer_desc : layers) {
        // Cancel early when parsing.
        if (obsolete()) {
            return;
        }

        if (layer_desc.child_layer.size()) {
            // This is a layer group.
            // TODO: create framebuffer
            parseStyleLayers(layer_desc.child_layer);
            // TODO: render framebuffer on previous framebuffer
        } else {
            // This is a singular layer. Check if this bucket already exists. If not,
            // parse this bucket.
            auto bucket_it = tile.buckets.find(layer_desc.bucket_name);
            if (bucket_it == tile.buckets.end()) {
                auto bucket_it = style.buckets.find(layer_desc.bucket_name);
                if (layer_desc.bucket_name == "background") {
                    // background is a special, fake bucket
                    continue;
                }
                else if (bucket_it != style.buckets.end()) {
                    // Only create the new bucket if we have an actual specification
                    // for it.
                    std::unique_ptr<Bucket> bucket = createBucket(bucket_it->second);
                    if (bucket) {
                        // Bucket creation might fail because the data tile may not
                        // contain any data that falls into this bucket.
                        tile.buckets[layer_desc.bucket_name] = std::move(bucket);
                    }
                } else {
                    // There is no proper specification for this bucket, even though
                    // it is referenced in the stylesheet.
                    fprintf(stderr, "Stylesheet specifies bucket %s, but it is not defined\n", layer_desc.bucket_name.c_str());
                }
            }
        }
    }
}

std::unique_ptr<Bucket> TileParser::createBucket(const BucketDescription& bucket_desc) {
    auto layer_it = vector_data.layers.find(bucket_desc.source_layer);
    if (layer_it != vector_data.layers.end()) {
        const VectorTileLayer& layer = layer_it->second;
        if (bucket_desc.type == BucketType::Fill) {
            return createFillBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Line) {
            return createLineBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Icon) {
            return createIconBucket(layer, bucket_desc);
        } else if (bucket_desc.type == BucketType::Text) {
            return createTextBucket(layer, bucket_desc);
        } else {
            throw std::runtime_error("unknown bucket type");
        }
    } else {
        // The layer specified in the bucket does not exist. Do nothing.
    }

    return nullptr;
}

template <class Bucket>
void TileParser::addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (pbf feature : filtered_layer) {
        if (obsolete()) return;

        while (feature.next(4)) { // geometry
            pbf geometry_pbf = feature.message();
            if (geometry_pbf) {
                bucket->addGeometry(geometry_pbf);
            }
        }
    }
}

template <class Bucket, typename ...Args>
void TileParser::addBucketFeatures(Bucket& bucket, const VectorTileLayer& layer, const BucketDescription& bucket_desc, Args&& ...args) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (const pbf& feature_pbf : filtered_layer) {
        if (obsolete()) return;
        bucket->addFeature({ feature_pbf, layer }, std::forward<Args>(args)...);
    }
}

std::unique_ptr<Bucket> TileParser::createFillBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::unique_ptr<FillBucket> bucket = std::make_unique<FillBucket>(
        tile.fillVertexBuffer, tile.triangleElementsBuffer, tile.lineElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createLineBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::unique_ptr<LineBucket> bucket = std::make_unique<LineBucket>(
        tile.lineVertexBuffer, tile.triangleElementsBuffer, tile.pointElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createIconBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {
    std::unique_ptr<IconBucket> bucket = std::make_unique<IconBucket>(
        tile.iconVertexBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc, spriteAtlas);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createTextBucket(const VectorTileLayer& layer, const BucketDescription& bucket_desc) {

    // Determine the correct text stack.
    if (!layer.shaping.size()) {
        return nullptr;
    }

    // TODO: currently hardcoded to use the first font stack.
    const std::map<Value, Shaping>& shaping = layer.shaping.begin()->second;

    const Faces& const_faces = faces;

    IndexedFaces faces;
    for (const std::string& face : layer.faces) {
        auto it = const_faces.find(face);
        if (it == const_faces.end()) {
            // This layer references an unknown face.
            return nullptr;
        }
        faces.push_back(&it->second);
    }

    std::unique_ptr<TextBucket> bucket = std::make_unique<TextBucket>(
        tile.textVertexBuffer, tile.triangleElementsBuffer, bucket_desc, placement);
    addBucketFeatures(bucket, layer, bucket_desc, faces, shaping);
    return std::move(bucket);
}
