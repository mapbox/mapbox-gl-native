#include <llmr/map/tile_parser.hpp>

#include <llmr/map/vector_tile_data.hpp>
#include <llmr/style/style.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/renderer/line_bucket.hpp>
#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/renderer/text_bucket.hpp>
#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/geometry/glyph_atlas.hpp>
#include <llmr/text/glyph_store.hpp>
#include <llmr/text/glyph.hpp>

#include <llmr/util/std.hpp>
#include <llmr/util/utf.hpp>

#ifdef __linux__
#include <boost/regex.hpp>
namespace regex_impl = boost;
#else
#include <regex>
namespace regex_impl = std;
#endif

using namespace llmr;

TileParser::TileParser(const std::string &data, VectorTileData &tile,
                       const std::shared_ptr<const Style> &style,
                       const std::shared_ptr<GlyphAtlas> &glyphAtlas,
                       const std::shared_ptr<GlyphStore> &glyphStore,
                       const std::shared_ptr<SpriteAtlas> &spriteAtlas)
    : vector_data(pbf((const uint8_t *)data.data(), data.size())),
      tile(tile),
      style(style),
      glyphAtlas(glyphAtlas),
      glyphStore(glyphStore),
      spriteAtlas(spriteAtlas),
      placement(tile.id.z) {
}

void TileParser::parse() {
    parseStyleLayers(style->layers);
}

bool TileParser::obsolete() const { return tile.state == TileData::State::obsolete; }

void TileParser::addGlyph(uint64_t tileid, const std::string stackname,
                          const std::u32string &string, const FontStack &fontStack,
                          GlyphAtlas &glyphAtlas, GlyphPositions &face) {
    std::map<uint32_t, SDFGlyph> sdfs = fontStack.getSDFs();
    // Loop through all characters and add glyph to atlas, positions.
    for (uint32_t chr : string) {
        const SDFGlyph sdf = sdfs[chr];
        const Rect<uint16_t> rect = glyphAtlas.addGlyph(tileid, stackname, sdf);
        face.emplace(chr, Glyph{rect, sdf.metrics});
    }
}

void TileParser::parseStyleLayers(const std::vector<LayerDescription> &layers) {
    for (const LayerDescription &layer_desc : layers) {
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
                auto bucket_it = style->buckets.find(layer_desc.bucket_name);
                if (layer_desc.bucket_name == "background") {
                    // background is a special, fake bucket
                    continue;
                } else if (bucket_it != style->buckets.end()) {
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
                    if (debug::tileParseWarnings) {
                        fprintf(stderr, "Stylesheet specifies bucket %s, but it is not defined\n",
                                layer_desc.bucket_name.c_str());
                    }
                }
            }
        }
    }
}

std::unique_ptr<Bucket> TileParser::createBucket(const BucketDescription &bucket_desc) {
    auto layer_it = vector_data.layers.find(bucket_desc.source_layer);
    if (layer_it != vector_data.layers.end()) {
        const VectorTileLayer &layer = layer_it->second;
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
        if (debug::tileParseWarnings) {
            fprintf(stderr, "[WARNING] layer '%s' does not exist in tile %d/%d/%d of source '%s'\n",
                    bucket_desc.source_layer.c_str(), tile.id.z, tile.id.x, tile.id.y,
                    bucket_desc.source_name.c_str());
        }
    }

    return nullptr;
}

template <class Bucket>
void TileParser::addBucketFeatures(Bucket &bucket, const VectorTileLayer &layer,
                                   const BucketDescription &bucket_desc) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
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

template <class Bucket, typename... Args>
void TileParser::addBucketFeatures(Bucket &bucket, const VectorTileLayer &layer,
                                   const BucketDescription &bucket_desc, Args &&... args) {
    FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
    for (const pbf &feature_pbf : filtered_layer) {
        if (obsolete())
            return;
        bucket->addFeature({feature_pbf, layer}, std::forward<Args>(args)...);
    }
}

std::unique_ptr<Bucket> TileParser::createFillBucket(const VectorTileLayer &layer,
                                                     const BucketDescription &bucket_desc) {
    std::unique_ptr<FillBucket> bucket = std::make_unique<FillBucket>(
        tile.fillVertexBuffer, tile.triangleElementsBuffer, tile.lineElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createLineBucket(const VectorTileLayer &layer,
                                                     const BucketDescription &bucket_desc) {
    std::unique_ptr<LineBucket> bucket = std::make_unique<LineBucket>(
        tile.lineVertexBuffer, tile.triangleElementsBuffer, tile.pointElementsBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createIconBucket(const VectorTileLayer &layer,
                                                     const BucketDescription &bucket_desc) {
    std::unique_ptr<IconBucket> bucket =
        std::make_unique<IconBucket>(tile.iconVertexBuffer, bucket_desc);
    addBucketFeatures(bucket, layer, bucket_desc, *spriteAtlas);
    return obsolete() ? nullptr : std::move(bucket);
}

typedef std::pair<uint16_t, uint16_t> GlyphRange;

std::unique_ptr<Bucket> TileParser::createTextBucket(const VectorTileLayer &layer,
                                                     const BucketDescription &bucket_desc) {
    std::unique_ptr<TextBucket> bucket = std::make_unique<TextBucket>(
        tile.textVertexBuffer, tile.triangleElementsBuffer, bucket_desc, placement);

    util::utf8_to_utf32 ucs4conv;
    regex_impl::regex token_regex("\\{\\{(\\w+)\\}\\}");
    const auto tokens_end = regex_impl::sregex_token_iterator();


    std::vector<std::pair<std::u32string, pbf>> labels;

    // Determine and load glyph ranges
    {
        std::set<GlyphRange> ranges;

        FilteredVectorTileLayer filtered_layer(layer, bucket_desc);
        for (const pbf &feature_pbf : filtered_layer) {
            if (obsolete())
                return nullptr;
            VectorTileFeature feature{feature_pbf, layer};

            const std::string &field = bucket_desc.geometry.field;
            std::string source_string;
            source_string.reserve(field.size());

            bool token = false;
            for (auto token_it = regex_impl::sregex_token_iterator(field.begin(), field.end(), token_regex, {-1, 1}); token_it != tokens_end; ++token_it, token = !token) {
                if (!token_it->matched) {
                    continue;
                }

                if (token) {
                    auto it_prop = feature.properties.find(token_it->str());
                    if (it_prop == feature.properties.end()) {
                        // feature does not have the correct property
                        if (debug::labelTextMissingWarning) {
                            fprintf(stderr,
                                    "[WARNING] feature doesn't have property '%s' required for labelling\n",
                                    token_it->str().c_str());
                        }
                        continue;
                    }
                    source_string += toString(it_prop->second);
                } else {
                    source_string += token_it->str();
                }
            }

            const std::u32string string = ucs4conv.convert(source_string);

            // Loop through all characters of this text and collect unique codepoints.
            for (uint32_t chr : string) {
                ranges.insert(getGlyphRange(chr));
            }

            labels.emplace_back(string, feature.geometry);
        }

        glyphStore->waitForGlyphRanges(bucket_desc.geometry.font, ranges);
    }

    // Create a copy!
    const FontStack &fontStack = glyphStore->getFontStack(bucket_desc.geometry.font);
    GlyphPositions face;

    // Shape and place all labels.
    for (const std::pair<std::u32string, pbf> &label : labels) {

        // Shape labels.
        const Shaping shaping = fontStack.getShaping(label.first, bucket_desc.geometry.max_width,
                bucket_desc.geometry.line_height, bucket_desc.geometry.alignment,
                bucket_desc.geometry.vertical_alignment, bucket_desc.geometry.letter_spacing);

        // Place labels.
        addGlyph(tile.id.to_uint64(), bucket_desc.geometry.font, label.first, fontStack, *glyphAtlas,
                 face);

        bucket->addFeature(label.second, face, shaping);
    }

    return std::move(bucket);
}
