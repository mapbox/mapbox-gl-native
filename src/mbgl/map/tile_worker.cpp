#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/tile_worker.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/constants.hpp>

using namespace mbgl;

TileWorker::TileWorker(TileID id_,
                       std::string sourceID_,
                       const uint16_t maxZoom_,
                       Style& style_,
                       std::vector<util::ptr<StyleLayer>> layers_,
                       const std::atomic<TileData::State>& state_,
                       std::unique_ptr<CollisionTile> collision_)
    : layers(std::move(layers_)),
      id(id_),
      sourceID(sourceID_),
      maxZoom(maxZoom_),
      style(style_),
      state(state_),
      collisionTile(std::move(collision_)) {
    assert(style.sprite);
}

TileWorker::~TileWorker() {
    style.glyphAtlas->removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

Bucket* TileWorker::getBucket(const StyleLayer& layer) const {
    std::lock_guard<std::mutex> lock(bucketsMutex);

    const auto it = buckets.find(layer.bucket->name);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

TileParseResult TileWorker::parse(const GeometryTile& geometryTile) {
    partialParse = false;

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        parseLayer(**i, geometryTile);
    }

    return partialParse ? TileData::State::partial : TileData::State::parsed;
}

void TileWorker::redoPlacement(float angle, float pitch, bool collisionDebug) {
    collisionTile = std::make_unique<CollisionTile>(angle, pitch, collisionDebug);
    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        auto bucket = getBucket(**i);
        if (bucket) {
            bucket->placeFeatures(*collisionTile);
        }
    }
}

template <typename T>
struct PropertyEvaluator {
    typedef T result_type;
    explicit PropertyEvaluator(float z_) : z(z_) {}

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
void applyLayoutProperty(PropertyKey key, const ClassProperties &classProperties, T &target, const float z) {
    auto it = classProperties.properties.find(key);
    if (it != classProperties.properties.end()) {
        const PropertyEvaluator<T> evaluator(z);
        target = mapbox::util::apply_visitor(evaluator, it->second);
    }
}

void TileWorker::parseLayer(const StyleLayer& layer, const GeometryTile& geometryTile) {
    // Cancel early when parsing.
    if (state == TileData::State::obsolete)
        return;

    // Background is a special case.
    if (layer.isBackground())
        return;

    if (!layer.bucket) {
        Log::Warning(Event::ParseTile, "layer '%s' does not have buckets", layer.id.c_str());
        return;
    }

    // This is a singular layer. Check if this bucket already exists.
    if (getBucket(layer))
        return;

    const StyleBucket& styleBucket = *layer.bucket;

    // Skip this bucket if we are to not render this
    if (styleBucket.source != sourceID)
        return;
    if (id.z < std::floor(styleBucket.min_zoom) && std::floor(styleBucket.min_zoom) < maxZoom)
        return;
    if (id.z >= std::ceil(styleBucket.max_zoom))
        return;
    if (styleBucket.visibility == mbgl::VisibilityType::None)
        return;

    auto geometryLayer = geometryTile.getLayer(styleBucket.source_layer);
    if (!geometryLayer) {
        // The layer specified in the bucket does not exist. Do nothing.
        if (debug::tileParseWarnings) {
            Log::Warning(Event::ParseTile, "layer '%s' does not exist in tile %d/%d/%d",
                    styleBucket.source_layer.c_str(), id.z, id.x, id.y);
        }
        return;
    }

    std::unique_ptr<Bucket> bucket;

    switch (styleBucket.type) {
    case StyleLayerType::Fill:
        bucket = createFillBucket(*geometryLayer, styleBucket);
        break;
    case StyleLayerType::Line:
        bucket = createLineBucket(*geometryLayer, styleBucket);
        break;
    case StyleLayerType::Circle:
        bucket = createCircleBucket(*geometryLayer, styleBucket);
        break;
    case StyleLayerType::Symbol:
        bucket = createSymbolBucket(*geometryLayer, styleBucket);
        break;
    case StyleLayerType::Raster:
        return;
    default:
        Log::Warning(Event::ParseTile, "unknown bucket render type for layer '%s' (source layer '%s')",
                styleBucket.name.c_str(), styleBucket.source_layer.c_str());
    }

    // Bucket creation might fail because the data tile may not
    // contain any data that falls into this bucket.
    if (!bucket)
        return;

    std::lock_guard<std::mutex> lock(bucketsMutex);
    buckets[styleBucket.name] = std::move(bucket);
}

template <class Bucket>
void TileWorker::addBucketGeometries(Bucket& bucket, const GeometryTileLayer& layer, const FilterExpression &filter) {
    for (std::size_t i = 0; i < layer.featureCount(); i++) {
        auto feature = layer.getFeature(i);

        if (state == TileData::State::obsolete)
            return;

        GeometryTileFeatureExtractor extractor(*feature);
        if (!evaluate(filter, extractor))
            continue;

        bucket->addGeometry(feature->getGeometries());
    }
}

std::unique_ptr<Bucket> TileWorker::createFillBucket(const GeometryTileLayer& layer,
                                                     const StyleBucket& bucket_desc) {
    auto bucket = std::make_unique<FillBucket>(fillVertexBuffer,
                                                triangleElementsBuffer,
                                                lineElementsBuffer);
    addBucketGeometries(bucket, layer, bucket_desc.filter);
    return bucket->hasData() ? std::move(bucket) : nullptr;
}

std::unique_ptr<Bucket> TileWorker::createLineBucket(const GeometryTileLayer& layer,
                                                     const StyleBucket& bucket_desc) {
    auto bucket = std::make_unique<LineBucket>(lineVertexBuffer,
                                                triangleElementsBuffer);

    const float z = id.z;
    auto& layout = bucket->layout;

    applyLayoutProperty(PropertyKey::LineCap, bucket_desc.layout, layout.cap, z);
    applyLayoutProperty(PropertyKey::LineJoin, bucket_desc.layout, layout.join, z);
    applyLayoutProperty(PropertyKey::LineMiterLimit, bucket_desc.layout, layout.miter_limit, z);
    applyLayoutProperty(PropertyKey::LineRoundLimit, bucket_desc.layout, layout.round_limit, z);

    addBucketGeometries(bucket, layer, bucket_desc.filter);
    return bucket->hasData() ? std::move(bucket) : nullptr;
}

std::unique_ptr<Bucket> TileWorker::createCircleBucket(const GeometryTileLayer& layer,
                                                       const StyleBucket& bucket_desc) {
    auto bucket = std::make_unique<CircleBucket>(circleVertexBuffer,
                                                 triangleElementsBuffer);

    // Circle does not have layout properties to apply.

    addBucketGeometries(bucket, layer, bucket_desc.filter);
    return bucket->hasData() ? std::move(bucket) : nullptr;
}

std::unique_ptr<Bucket> TileWorker::createSymbolBucket(const GeometryTileLayer& layer,
                                                       const StyleBucket& bucket_desc) {
    auto bucket = std::make_unique<SymbolBucket>(id.overscaling, id.z);

    const float z = id.z;
    auto& layout = bucket->layout;

    applyLayoutProperty(PropertyKey::SymbolPlacement, bucket_desc.layout, layout.placement, z);
    if (layout.placement == PlacementType::Line) {
        layout.icon.rotation_alignment = RotationAlignmentType::Map;
        layout.text.rotation_alignment = RotationAlignmentType::Map;
    };
    applyLayoutProperty(PropertyKey::SymbolSpacing, bucket_desc.layout, layout.spacing, z);
    applyLayoutProperty(PropertyKey::SymbolAvoidEdges, bucket_desc.layout, layout.avoid_edges, z);

    applyLayoutProperty(PropertyKey::IconAllowOverlap, bucket_desc.layout, layout.icon.allow_overlap, z);
    applyLayoutProperty(PropertyKey::IconIgnorePlacement, bucket_desc.layout, layout.icon.ignore_placement, z);
    applyLayoutProperty(PropertyKey::IconOptional, bucket_desc.layout, layout.icon.optional, z);
    applyLayoutProperty(PropertyKey::IconRotationAlignment, bucket_desc.layout, layout.icon.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::IconImage, bucket_desc.layout, layout.icon.image, z);
    applyLayoutProperty(PropertyKey::IconPadding, bucket_desc.layout, layout.icon.padding, z);
    applyLayoutProperty(PropertyKey::IconRotate, bucket_desc.layout, layout.icon.rotate, z);
    applyLayoutProperty(PropertyKey::IconKeepUpright, bucket_desc.layout, layout.icon.keep_upright, z);
    applyLayoutProperty(PropertyKey::IconOffset, bucket_desc.layout, layout.icon.offset, z);

    applyLayoutProperty(PropertyKey::TextRotationAlignment, bucket_desc.layout, layout.text.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::TextField, bucket_desc.layout, layout.text.field, z);
    applyLayoutProperty(PropertyKey::TextFont, bucket_desc.layout, layout.text.font, z);
    applyLayoutProperty(PropertyKey::TextMaxWidth, bucket_desc.layout, layout.text.max_width, z);
    applyLayoutProperty(PropertyKey::TextLineHeight, bucket_desc.layout, layout.text.line_height, z);
    applyLayoutProperty(PropertyKey::TextLetterSpacing, bucket_desc.layout, layout.text.letter_spacing, z);
    applyLayoutProperty(PropertyKey::TextMaxAngle, bucket_desc.layout, layout.text.max_angle, z);
    applyLayoutProperty(PropertyKey::TextRotate, bucket_desc.layout, layout.text.rotate, z);
    applyLayoutProperty(PropertyKey::TextPadding, bucket_desc.layout, layout.text.padding, z);
    applyLayoutProperty(PropertyKey::TextIgnorePlacement, bucket_desc.layout, layout.text.ignore_placement, z);
    applyLayoutProperty(PropertyKey::TextOptional, bucket_desc.layout, layout.text.optional, z);
    applyLayoutProperty(PropertyKey::TextJustify, bucket_desc.layout, layout.text.justify, z);
    applyLayoutProperty(PropertyKey::TextAnchor, bucket_desc.layout, layout.text.anchor, z);
    applyLayoutProperty(PropertyKey::TextKeepUpright, bucket_desc.layout, layout.text.keep_upright, z);
    applyLayoutProperty(PropertyKey::TextTransform, bucket_desc.layout, layout.text.transform, z);
    applyLayoutProperty(PropertyKey::TextOffset, bucket_desc.layout, layout.text.offset, z);
    applyLayoutProperty(PropertyKey::TextAllowOverlap, bucket_desc.layout, layout.text.allow_overlap, z);

    applyLayoutProperty(PropertyKey::IconSize, bucket_desc.layout, layout.icon.size, z + 1);
    applyLayoutProperty(PropertyKey::IconSize, bucket_desc.layout, layout.icon.max_size, 18);
    applyLayoutProperty(PropertyKey::TextSize, bucket_desc.layout, layout.text.size, z + 1);
    applyLayoutProperty(PropertyKey::TextSize, bucket_desc.layout, layout.text.max_size, 18);

    if (bucket->needsDependencies(layer, bucket_desc.filter, *style.glyphStore, *style.sprite)) {
        partialParse = true;
    }

    // We do not proceed if the parser is in a "partial" state because
    // the layer ordering needs to be respected when calculating text
    // collisions. Although, at this point, we requested all the resources
    // needed by this tile.
    if (partialParse) {
        return nullptr;
    }

    bucket->addFeatures(reinterpret_cast<uintptr_t>(this), *style.spriteAtlas, *style.glyphAtlas,
                        *style.glyphStore, *collisionTile);

    return bucket->hasData() ? std::move(bucket) : nullptr;
}
