#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/tile_worker.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/interactive_features_impl.hpp>

using namespace mbgl;

TileWorker::TileWorker(TileID id_,
                       std::string sourceID_,
                       Style& style_,
                       const std::atomic<TileData::State>& state_)
    : id(id_),
      sourceID(sourceID_),
      style(style_),
      state(state_) {
}

TileWorker::~TileWorker() {
    style.glyphAtlas->removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

TileParseResult TileWorker::parseAllLayers(std::vector<util::ptr<StyleLayer>> layers,
                                           const GeometryTile& geometryTile,
                                           PlacementConfig config) {
    // We're doing a fresh parse of the tile, because the underlying data has changed.
    pending.clear();
    partialParse = false;

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
            if (!symbolBucket->needsDependencies(*style.glyphStore, *style.spriteStore)) {
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

    StyleBucketParameters parameters(id,
                                     *geometryLayer,
                                     state,
                                     reinterpret_cast<uintptr_t>(this),
                                     partialParse,
                                     *style.spriteAtlas,
                                     *style.spriteStore,
                                     *style.glyphAtlas,
                                     *style.glyphStore,
                                     *collisionTile);

    std::unique_ptr<Bucket> bucket = layer.createBucket(parameters);

    if (layer.isInteractive) {
        for (std::size_t i = 0; i < geometryLayer->featureCount(); i++) {
            const auto feature = geometryLayer->getFeature(i);
            FeatureBox featureBox = {{ extent, extent }, { -1, -1 }};
            const auto geometries = feature->getGeometries();
            for (std::size_t j = 0; j < geometries.size(); j++) {
                const auto geometry = geometries.at(j);
                for (std::size_t k = 0; k < geometry.size(); k++) {
                    auto point = geometry.at(k);
                    if (point.x < 0 || point.x > extent - 1 || point.y < 0 || point.y > extent - 1) continue;
                    const auto min = featureBox.min_corner();
                    const auto max = featureBox.max_corner();
                    if (point.x < min.get<0>()) {
                        featureBox.min_corner().set<0>(::fmax(point.x, 0));
                    }
                    if (point.y < min.get<1>()) {
                        featureBox.min_corner().set<1>(::fmax(point.y, 0));
                    }
                    if (point.x > max.get<0>()) {
                        featureBox.max_corner().set<0>(::fmin(point.x, extent - 1));
                    }
                    if (point.y > max.get<1>()) {
                        featureBox.max_corner().set<1>(::fmin(point.y, extent - 1));
                    }
                }
            }

            if (featureBox.min_corner().get<0>() < extent && featureBox.min_corner().get<1>() < extent &&
                featureBox.max_corner().get<0>() > -1 && featureBox.max_corner().get<1>() > -1) {
                const auto& values = feature->getAllValues();
                FeatureProperties properties;
                properties.insert(values.begin(), values.end());
                result.featureTree.insert(std::make_tuple(featureBox, layer.id, properties));
            }
        }
    }

    if (layer.type == StyleLayerType::Symbol && partialParse) {
        // We cannot parse this bucket yet. Instead, we're saving it for later.
        pending.emplace_back(layer, std::move(bucket));
    } else {
        insertBucket(layer.bucketName(), std::move(bucket));
    }
}

void TileWorker::insertBucket(const std::string& name, std::unique_ptr<Bucket> bucket) {
    if (bucket->hasData()) {
        result.buckets.emplace(name, std::move(bucket));
    }
}
