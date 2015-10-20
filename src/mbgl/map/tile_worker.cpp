#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/tile_worker.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>

using namespace mbgl;

TileWorker::TileWorker(TileID id_,
                       std::string sourceID_,
                       Style& style_,
                       const std::atomic<TileData::State>& state_)
    : id(id_),
      sourceID(sourceID_),
      parameters(id.z),
      style(style_),
      state(state_) {
    assert(style.sprite);
}

TileWorker::~TileWorker() {
    style.glyphAtlas->removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

TileParseResult TileWorker::parseAllLayers(std::vector<util::ptr<StyleLayer>> layers,
                                           const GeometryTile& geometryTile,
                                           PlacementConfig config) {
    // We're doing a fresh parse of the tile, because the underlying data has changed.
    pending.clear();

    // Reset the collision tile so we have a clean slate; we're placing all features anyway.
    collisionTile = std::make_unique<CollisionTile>(config);

    // We're storing a set of bucket names we've parsed to avoid parsing a bucket twice that is
    // referenced from more than one layer
    std::set<std::string> parsed;

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        const StyleLayer& layer = **i;
        if (parsed.find(layer.bucketName()) == parsed.end()) {
            parsed.emplace(layer.bucketName());
            parseLayer(layer, geometryTile);
        }
    }

    result.state = pending.empty() ? TileData::State::parsed : TileData::State::partial;
    return std::move(result);
}

TileParseResult TileWorker::parsePendingLayers() {
    // Try parsing the remaining layers that we couldn't parse in the first step due to missing
    // dependencies.
    for (auto it = pending.begin(); it != pending.end();) {
        auto& layer = it->first;
        auto& bucket = it->second;
        assert(bucket);

        if (layer.type == StyleLayerType::Symbol) {
            auto symbolBucket = dynamic_cast<SymbolBucket*>(bucket.get());
            if (!symbolBucket->needsDependencies(*style.glyphStore, *style.sprite)) {
                symbolBucket->addFeatures(reinterpret_cast<uintptr_t>(this), *style.spriteAtlas,
                                          *style.glyphAtlas, *style.glyphStore, *collisionTile);
                insertBucket(layer.bucketName(), std::move(bucket));
                pending.erase(it++);
                continue;
            }
        }

        // Advance the iterator here; we're skipping this when erasing an element from this list.
        ++it;
    }

    result.state = pending.empty() ? TileData::State::parsed : TileData::State::partial;
    return std::move(result);
}

void TileWorker::redoPlacement(
    std::vector<util::ptr<StyleLayer>> layers,
    const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
    PlacementConfig config) {

    // Reset the collision tile so we have a clean slate; we're placing all features anyway.
    collisionTile = std::make_unique<CollisionTile>(config);

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        const auto it = buckets->find((*i)->id);
        if (it != buckets->end()) {
            it->second->placeFeatures(*collisionTile);
        }
    }
}

template <typename T>
void applyLayoutProperty(PropertyKey key, const ClassProperties &classProperties, T &target, const StyleCalculationParameters& parameters) {
    auto it = classProperties.properties.find(key);
    if (it != classProperties.properties.end()) {
        const PropertyEvaluator<T> evaluator(parameters);
        target = mapbox::util::apply_visitor(evaluator, it->second);
    }
}

void TileWorker::parseLayer(const StyleLayer& layer, const GeometryTile& geometryTile) {
    // Cancel early when parsing.
    if (state == TileData::State::obsolete)
        return;

    // Background is a special case.
    if (layer.type == StyleLayerType::Background)
        return;

    // Skip this bucket if we are to not render this
    if ((layer.source != sourceID) ||
        (id.z < std::floor(layer.minZoom)) ||
        (id.z >= std::ceil(layer.maxZoom)) ||
        (layer.visibility == VisibilityType::None)) {
        return;
    }

    auto geometryLayer = geometryTile.getLayer(layer.sourceLayer);
    if (!geometryLayer) {
        // The layer specified in the bucket does not exist. Do nothing.
        if (debug::tileParseWarnings) {
            Log::Warning(Event::ParseTile, "layer '%s' does not exist in tile %d/%d/%d",
                    layer.sourceLayer.c_str(), id.z, id.x, id.y);
        }
        return;
    }

    switch (layer.type) {
    case StyleLayerType::Fill:
        createFillBucket(*geometryLayer, layer);
        break;
    case StyleLayerType::Line:
        createLineBucket(*geometryLayer, layer);
        break;
    case StyleLayerType::Circle:
        createCircleBucket(*geometryLayer, layer);
        break;
    case StyleLayerType::Symbol:
        createSymbolBucket(*geometryLayer, layer);
        break;
    case StyleLayerType::Raster:
        break;
    default:
        Log::Warning(Event::ParseTile, "unknown bucket render type for layer '%s' (source layer '%s')",
                layer.id.c_str(), layer.sourceLayer.c_str());
    }
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

void TileWorker::createFillBucket(const GeometryTileLayer& tileLayer,
                                  const StyleLayer& layer) {
    auto bucket = std::make_unique<FillBucket>();

    // Fill does not have layout properties to apply.

    addBucketGeometries(bucket, tileLayer, layer.filter);

    insertBucket(layer.bucketName(), std::move(bucket));
}

void TileWorker::createLineBucket(const GeometryTileLayer& tileLayer,
                                  const StyleLayer& layer) {
    auto bucket = std::make_unique<LineBucket>();

    const float z = id.z;
    auto& layout = bucket->layout;

    applyLayoutProperty(PropertyKey::LineCap, layer.layout, layout.cap, z);
    applyLayoutProperty(PropertyKey::LineJoin, layer.layout, layout.join, z);
    applyLayoutProperty(PropertyKey::LineMiterLimit, layer.layout, layout.miter_limit, z);
    applyLayoutProperty(PropertyKey::LineRoundLimit, layer.layout, layout.round_limit, z);

    addBucketGeometries(bucket, tileLayer, layer.filter);

    insertBucket(layer.bucketName(), std::move(bucket));
}

void TileWorker::createCircleBucket(const GeometryTileLayer& tileLayer,
                                    const StyleLayer& layer) {
    auto bucket = std::make_unique<CircleBucket>();

    // Circle does not have layout properties to apply.

    addBucketGeometries(bucket, tileLayer, layer.filter);

    insertBucket(layer.bucketName(), std::move(bucket));
}

void TileWorker::createSymbolBucket(const GeometryTileLayer& tileLayer,
                                    const StyleLayer& layer) {
    auto bucket = std::make_unique<SymbolBucket>(id.overscaling, id.z);

    const float z = id.z;
    auto& layout = bucket->layout;

    applyLayoutProperty(PropertyKey::SymbolPlacement, layer.layout, layout.placement, z);
    if (layout.placement == PlacementType::Line) {
        layout.icon.rotation_alignment = RotationAlignmentType::Map;
        layout.text.rotation_alignment = RotationAlignmentType::Map;
    };
    applyLayoutProperty(PropertyKey::SymbolSpacing, layer.layout, layout.spacing, z);
    applyLayoutProperty(PropertyKey::SymbolAvoidEdges, layer.layout, layout.avoid_edges, z);

    applyLayoutProperty(PropertyKey::IconAllowOverlap, layer.layout, layout.icon.allow_overlap, z);
    applyLayoutProperty(PropertyKey::IconIgnorePlacement, layer.layout, layout.icon.ignore_placement, z);
    applyLayoutProperty(PropertyKey::IconOptional, layer.layout, layout.icon.optional, z);
    applyLayoutProperty(PropertyKey::IconRotationAlignment, layer.layout, layout.icon.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::IconImage, layer.layout, layout.icon.image, z);
    applyLayoutProperty(PropertyKey::IconPadding, layer.layout, layout.icon.padding, z);
    applyLayoutProperty(PropertyKey::IconRotate, layer.layout, layout.icon.rotate, z);
    applyLayoutProperty(PropertyKey::IconKeepUpright, layer.layout, layout.icon.keep_upright, z);
    applyLayoutProperty(PropertyKey::IconOffset, layer.layout, layout.icon.offset, z);

    applyLayoutProperty(PropertyKey::TextRotationAlignment, layer.layout, layout.text.rotation_alignment, z);
    applyLayoutProperty(PropertyKey::TextField, layer.layout, layout.text.field, z);
    applyLayoutProperty(PropertyKey::TextFont, layer.layout, layout.text.font, z);
    applyLayoutProperty(PropertyKey::TextMaxWidth, layer.layout, layout.text.max_width, z);
    applyLayoutProperty(PropertyKey::TextLineHeight, layer.layout, layout.text.line_height, z);
    applyLayoutProperty(PropertyKey::TextLetterSpacing, layer.layout, layout.text.letter_spacing, z);
    applyLayoutProperty(PropertyKey::TextMaxAngle, layer.layout, layout.text.max_angle, z);
    applyLayoutProperty(PropertyKey::TextRotate, layer.layout, layout.text.rotate, z);
    applyLayoutProperty(PropertyKey::TextPadding, layer.layout, layout.text.padding, z);
    applyLayoutProperty(PropertyKey::TextIgnorePlacement, layer.layout, layout.text.ignore_placement, z);
    applyLayoutProperty(PropertyKey::TextOptional, layer.layout, layout.text.optional, z);
    applyLayoutProperty(PropertyKey::TextJustify, layer.layout, layout.text.justify, z);
    applyLayoutProperty(PropertyKey::TextAnchor, layer.layout, layout.text.anchor, z);
    applyLayoutProperty(PropertyKey::TextKeepUpright, layer.layout, layout.text.keep_upright, z);
    applyLayoutProperty(PropertyKey::TextTransform, layer.layout, layout.text.transform, z);
    applyLayoutProperty(PropertyKey::TextOffset, layer.layout, layout.text.offset, z);
    applyLayoutProperty(PropertyKey::TextAllowOverlap, layer.layout, layout.text.allow_overlap, z);

    applyLayoutProperty(PropertyKey::IconSize, layer.layout, layout.icon.size, z + 1);
    applyLayoutProperty(PropertyKey::IconSize, layer.layout, layout.icon.max_size, 18);
    applyLayoutProperty(PropertyKey::TextSize, layer.layout, layout.text.size, z + 1);
    applyLayoutProperty(PropertyKey::TextSize, layer.layout, layout.text.max_size, 18);

    bucket->parseFeatures(tileLayer, layer.filter);

    assert(style.glyphStore);
    assert(style.sprite);
    const bool needsDependencies = bucket->needsDependencies(*style.glyphStore, *style.sprite);
    if (needsDependencies) {
        // We cannot parse this bucket yet. Instead, we're saving it for later.
        pending.emplace_back(layer, std::move(bucket));
    } else {
        assert(style.spriteAtlas);
        assert(style.glyphAtlas);
        assert(style.glyphStore);
        assert(collisionTile);
        bucket->addFeatures(reinterpret_cast<uintptr_t>(this), *style.spriteAtlas,
                            *style.glyphAtlas, *style.glyphStore, *collisionTile);
        insertBucket(layer.bucketName(), std::move(bucket));
    }
}

void TileWorker::insertBucket(const std::string& name, std::unique_ptr<Bucket> bucket) {
    if (bucket->hasData()) {
        result.buckets.emplace(name, std::move(bucket));
    }
}
