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

namespace mbgl {

// Note: This destructor is seemingly empty, but we need to declare it anyway
// because this object has a std::unique_ptr<> of a forward-declare type in
// its header file.
TileParser::~TileParser() = default;

TileParser::TileParser(const std::string &data, VectorTileData &tile_,
                       const util::ptr<const Style> &style_,
                       GlyphAtlas & glyphAtlas_,
                       GlyphStore & glyphStore_,
                       SpriteAtlas & spriteAtlas_,
                       const util::ptr<Sprite> &sprite_,
                       TexturePool& texturePool_)
    : vector_data(pbf((const uint8_t *)data.data(), data.size())),
      tile(tile_),
      style(style_),
      glyphAtlas(glyphAtlas_),
      glyphStore(glyphStore_),
      spriteAtlas(spriteAtlas_),
      sprite(sprite_),
      texturePool(texturePool_),
      collision(util::make_unique<Collision>(tile.id.z, 4096, tile.source.tile_size, tile.depth)) {
    assert(&tile != nullptr);
    assert(style);
    assert(sprite);
    assert(collision);
}

void TileParser::parse() {
    parseStyleLayers(style->layers);
}

bool TileParser::obsolete() const { return tile.state == TileData::State::obsolete; }

void TileParser::parseStyleLayers(util::ptr<StyleLayerGroup> group) {
    if (!group) {
        return;
    }

    for (const util::ptr<StyleLayer> &layer_desc : group->layers) {
        // Cancel early when parsing.
        if (obsolete()) {
            return;
        }

        if (layer_desc->isBackground()) {
            // background is a special, fake bucket
            continue;
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
            fprintf(stderr, "[WARNING] layer '%s' does not have buckets\n", layer_desc->id.c_str());
        }
    }
}

template <typename T>
struct PropertyEvaluator {
    typedef T result_type;
    PropertyEvaluator(float z_) : z(z_) {}

    template <typename P, typename std::enable_if<std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &value) const {
        return value;
    }

    T operator()(const Function<T> &value) const {
        return mapbox::util::apply_visitor(FunctionEvaluator<T>(z), value);
    }

    template <typename P, typename std::enable_if<!std::is_convertible<P, T>::value, int>::type = 0>
    T operator()(const P &) const {
        return T();
    }

private:
    const float z;
};

template <typename T>
void TileParser::applyLayoutProperty(PropertyKey key, ClassProperties &classProperties, T &target, const float z) {
    auto it = classProperties.properties.find(key);
    if (it != classProperties.properties.end()) {
        const PropertyEvaluator<T> evaluator(z);
        target = mapbox::util::apply_visitor(evaluator, it->second);

    }
}

template <>
void TileParser::applyLayoutProperties<FillProperties>(StyleBucket &bucket_desc, const float) {
    bucket_desc.render.set<StyleBucketFill>();
    // no-op; Fill buckets don't currently have any applicable layout properties
}

template<>
void TileParser::applyLayoutProperties<LineProperties>(StyleBucket &bucket_desc, const float z) {
    bucket_desc.render.set<StyleBucketLine>();
    StyleBucketLine &line = bucket_desc.render.get<StyleBucketLine>();
    applyLayoutProperty(PropertyKey::LineCap, bucket_desc.layout, line.cap, z);
    applyLayoutProperty(PropertyKey::LineJoin, bucket_desc.layout, line.join, z);
    applyLayoutProperty(PropertyKey::LineMiterLimit, bucket_desc.layout, line.miter_limit, z);
    applyLayoutProperty(PropertyKey::LineRoundLimit, bucket_desc.layout, line.round_limit, z);
}

template<>
void TileParser::applyLayoutProperties<SymbolProperties>(StyleBucket &bucket_desc, const float z) {
    bucket_desc.render.set<StyleBucketSymbol>();
    StyleBucketSymbol &symbol = bucket_desc.render.get<StyleBucketSymbol>();
    applyLayoutProperty(PropertyKey::SymbolPlacement, bucket_desc.layout, symbol.placement, z);
    if (symbol.placement == PlacementType::Line) {
        symbol.icon.rotation_alignment = RotationAlignmentType::Map;
        symbol.text.rotation_alignment = RotationAlignmentType::Map;
    };
    applyLayoutProperty(PropertyKey::SymbolMinDistance, bucket_desc.layout, symbol.min_distance, z);
    applyLayoutProperty(PropertyKey::SymbolAvoidEdges, bucket_desc.layout, symbol.avoid_edges, z);

    applyLayoutProperty(PropertyKey::IconAllowOverlap, bucket_desc.layout, symbol.icon.allow_overlap, z);
    applyLayoutProperty(PropertyKey::IconIgnorePlacement, bucket_desc.layout, symbol.icon.ignore_placement, z);
    applyLayoutProperty(PropertyKey::IconOptional, bucket_desc.layout, symbol.icon.optional, z);
    applyLayoutProperty(PropertyKey::IconRotationAlignment, bucket_desc.layout, symbol.icon.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::IconMaxSize, bucket_desc.layout, symbol.icon.max_size, z);
    applyLayoutProperty(PropertyKey::IconImage, bucket_desc.layout, symbol.icon.image, z);
    applyLayoutProperty(PropertyKey::IconPadding, bucket_desc.layout, symbol.icon.padding, z);
    applyLayoutProperty(PropertyKey::IconRotate, bucket_desc.layout, symbol.icon.rotate, z);
    applyLayoutProperty(PropertyKey::IconKeepUpright, bucket_desc.layout, symbol.icon.keep_upright, z);
    applyLayoutProperty(PropertyKey::IconOffset, bucket_desc.layout, symbol.icon.offset, z);

    applyLayoutProperty(PropertyKey::TextRotationAlignment, bucket_desc.layout, symbol.text.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::TextField, bucket_desc.layout, symbol.text.field, z);
    applyLayoutProperty(PropertyKey::TextFont, bucket_desc.layout, symbol.text.font, z);
    applyLayoutProperty(PropertyKey::TextMaxSize, bucket_desc.layout, symbol.text.max_size, z);
    applyLayoutProperty(PropertyKey::TextMaxWidth, bucket_desc.layout, symbol.text.max_width, z);
    applyLayoutProperty(PropertyKey::TextLineHeight, bucket_desc.layout, symbol.text.line_height, z);
    applyLayoutProperty(PropertyKey::TextLetterSpacing, bucket_desc.layout, symbol.text.letter_spacing, z);
    applyLayoutProperty(PropertyKey::TextMaxAngle, bucket_desc.layout, symbol.text.max_angle, z);
    applyLayoutProperty(PropertyKey::TextRotate, bucket_desc.layout, symbol.text.rotate, z);
    applyLayoutProperty(PropertyKey::TextPadding, bucket_desc.layout, symbol.text.padding, z);
    applyLayoutProperty(PropertyKey::TextIgnorePlacement, bucket_desc.layout, symbol.text.ignore_placement, z);
    applyLayoutProperty(PropertyKey::TextOptional, bucket_desc.layout, symbol.text.optional, z);
    applyLayoutProperty(PropertyKey::TextJustify, bucket_desc.layout, symbol.text.justify, z);
    applyLayoutProperty(PropertyKey::TextAnchor, bucket_desc.layout, symbol.text.anchor, z);
    applyLayoutProperty(PropertyKey::TextKeepUpright, bucket_desc.layout, symbol.text.keep_upright, z);
    applyLayoutProperty(PropertyKey::TextTransform, bucket_desc.layout, symbol.text.transform, z);
    applyLayoutProperty(PropertyKey::TextOffset, bucket_desc.layout, symbol.text.offset, z);
    applyLayoutProperty(PropertyKey::TextAllowOverlap, bucket_desc.layout, symbol.text.allow_overlap, z);
}

std::unique_ptr<Bucket> TileParser::createBucket(util::ptr<StyleBucket> bucket_desc) {
    if (!bucket_desc) {
        fprintf(stderr, "missing bucket desc\n");
        return nullptr;
    }

    // Skip this bucket if we are to not render this
    if (tile.id.z < std::floor(bucket_desc->min_zoom) && std::floor(bucket_desc->min_zoom) < tile.source.max_zoom) return nullptr;
    if (tile.id.z >= std::ceil(bucket_desc->max_zoom)) return nullptr;
    if (bucket_desc->visibility == mbgl::VisibilityType::None) return nullptr;

    auto layer_it = vector_data.layers.find(bucket_desc->source_layer);
    if (layer_it != vector_data.layers.end()) {
        const VectorTileLayer &layer = layer_it->second;
        if (bucket_desc->type == StyleLayerType::Fill) {
            applyLayoutProperties<FillProperties>(*bucket_desc, tile.id.z);
            return createFillBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketFill>());
        } else if (bucket_desc->type == StyleLayerType::Line) {
            applyLayoutProperties<LineProperties>(*bucket_desc, tile.id.z);
            return createLineBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketLine>());
        } else if (bucket_desc->type == StyleLayerType::Symbol) {
            applyLayoutProperties<SymbolProperties>(*bucket_desc, tile.id.z);
            return createSymbolBucket(layer, bucket_desc->filter, bucket_desc->render.get<StyleBucketSymbol>());
        } else if (bucket_desc->type == StyleLayerType::Raster) {
            return nullptr;
        } else {
            fprintf(stderr, "[WARNING] unknown bucket render type for layer '%s' (source layer '%s')\n", bucket_desc->name.c_str(), bucket_desc->source_layer.c_str());
        }
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
    std::unique_ptr<FillBucket> bucket = util::make_unique<FillBucket>(tile.fillVertexBuffer, tile.triangleElementsBuffer, tile.lineElementsBuffer, fill);
    addBucketGeometries(bucket, layer, filter);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createRasterBucket(const StyleBucketRaster &raster) {
    std::unique_ptr<RasterBucket> bucket = util::make_unique<RasterBucket>(texturePool, raster);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createLineBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketLine &line) {
    std::unique_ptr<LineBucket> bucket = util::make_unique<LineBucket>(tile.lineVertexBuffer, tile.triangleElementsBuffer, tile.pointElementsBuffer, line);
    addBucketGeometries(bucket, layer, filter);
    return obsolete() ? nullptr : std::move(bucket);
}

std::unique_ptr<Bucket> TileParser::createSymbolBucket(const VectorTileLayer& layer, const FilterExpression &filter, const StyleBucketSymbol &symbol) {
    std::unique_ptr<SymbolBucket> bucket = util::make_unique<SymbolBucket>(symbol, *collision);
    bucket->addFeatures(layer, filter, tile.id, spriteAtlas, *sprite, glyphAtlas, glyphStore);
    return obsolete() ? nullptr : std::move(bucket);
}

}
