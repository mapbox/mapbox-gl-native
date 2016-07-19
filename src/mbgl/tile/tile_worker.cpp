#include <mbgl/text/collision_tile.hpp>
#include <mbgl/tile/tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/exception.hpp>

#include <utility>

namespace mbgl {

using namespace style;

TileWorker::TileWorker(OverscaledTileID id_,
                       SpriteStore& spriteStore_,
                       GlyphAtlas& glyphAtlas_,
                       GlyphStore& glyphStore_,
                       const std::atomic<bool>& obsolete_,
                       const MapMode mode_)
    : id(std::move(id_)),
      spriteStore(spriteStore_),
      glyphAtlas(glyphAtlas_),
      glyphStore(glyphStore_),
      obsolete(obsolete_),
      mode(mode_) {
}

TileWorker::~TileWorker() {
    glyphAtlas.removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

TileParseResult TileWorker::parseAllLayers(std::vector<std::unique_ptr<Layer>> layers_,
                                           std::unique_ptr<const GeometryTileData> tileData_,
                                           PlacementConfig config) {
    // We're doing a fresh parse of the tile, because the underlying data has changed.
    pending.clear();
    placementPending.clear();
    partialParse = false;
    featureIndex = std::make_unique<FeatureIndex>();
    tileData = std::move(tileData_);

    // Store the layers for use in redoPlacement.
    layers = std::move(layers_);

    // We're storing a set of bucket names we've parsed to avoid parsing a bucket twice that is
    // referenced from more than one layer
    std::set<std::string> parsed;

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        const Layer* layer = i->get();
        if (parsed.find(layer->baseImpl->bucketName()) == parsed.end()) {
            parsed.emplace(layer->baseImpl->bucketName());
            parseLayer(layer);
        }
        featureIndex->addBucketLayerName(layer->baseImpl->bucketName(), layer->baseImpl->id);
    }

    return prepareResult(config);
}

TileParseResult TileWorker::parsePendingLayers(const PlacementConfig config) {
    // Try parsing the remaining layers that we couldn't parse in the first step due to missing
    // dependencies.
    for (auto it = pending.begin(); it != pending.end();) {
        const SymbolLayer& symbolLayer = *it->first;
        SymbolBucket* symbolBucket = dynamic_cast<SymbolBucket*>(it->second.get());

        if (!symbolBucket->needsDependencies(glyphStore, spriteStore)) {
            symbolBucket->addFeatures(reinterpret_cast<uintptr_t>(this),
                                      *symbolLayer.impl->spriteAtlas,
                                      glyphAtlas,
                                      glyphStore);
            placementPending.emplace(symbolLayer.impl->bucketName(), std::move(it->second));
            pending.erase(it++);
            continue;
        }

        // Advance the iterator here; we're skipping this when erasing an element from this list.
        ++it;
    }

    return prepareResult(config);
}

TileParseResult TileWorker::prepareResult(const PlacementConfig& config) {
    result.complete = pending.empty();

    if (result.complete) {
        featureIndex->setCollisionTile(placeLayers(config));
        result.featureIndex = std::move(featureIndex);
        result.tileData = std::move(tileData);
    }

    return std::move(result);
}

std::unique_ptr<CollisionTile> TileWorker::placeLayers(const PlacementConfig config) {
    auto collisionTile = redoPlacement(&placementPending, config);
    for (auto &p : placementPending) {
        p.second->swapRenderData();
        insertBucket(p.first, std::move(p.second));
    }
    placementPending.clear();
    return collisionTile;
}

std::unique_ptr<CollisionTile> TileWorker::redoPlacement(
    const std::unordered_map<std::string, std::unique_ptr<Bucket>>* buckets,
    PlacementConfig config) {

    auto collisionTile = std::make_unique<CollisionTile>(config);

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        const auto it = buckets->find((*i)->baseImpl->id);
        if (it != buckets->end()) {
            it->second->placeFeatures(*collisionTile);
        }
    }

    return collisionTile;
}

void TileWorker::parseLayer(const Layer* layer) {
    // Cancel early when parsing.
    if (obsolete)
        return;

    auto geometryLayer = tileData->getLayer(layer->baseImpl->sourceLayer);
    if (!geometryLayer) {
        // The layer specified in the bucket does not exist. Do nothing.
        if (debug::tileParseWarnings) {
            Log::Warning(Event::ParseTile, "layer '%s' does not exist in tile %s",
                    layer->baseImpl->sourceLayer.c_str(), util::toString(id).c_str());
        }
        return;
    }

    BucketParameters parameters(id,
                                     *geometryLayer,
                                     obsolete,
                                     reinterpret_cast<uintptr_t>(this),
                                     partialParse,
                                     spriteStore,
                                     glyphAtlas,
                                     glyphStore,
                                     *featureIndex,
                                     mode);

    std::unique_ptr<Bucket> bucket = layer->baseImpl->createBucket(parameters);

    if (layer->is<SymbolLayer>()) {
        if (partialParse) {
            // We cannot parse this bucket yet. Instead, we're saving it for later.
            pending.emplace_back(layer->as<SymbolLayer>(), std::move(bucket));
        } else {
            placementPending.emplace(layer->baseImpl->bucketName(), std::move(bucket));
        }
    } else {
        insertBucket(layer->baseImpl->bucketName(), std::move(bucket));
    }
}

void TileWorker::insertBucket(const std::string& name, std::unique_ptr<Bucket> bucket) {
    if (bucket->hasData()) {
        result.buckets.emplace(name, std::move(bucket));
    }
}

} // namespace mbgl
