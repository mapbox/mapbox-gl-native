#include <mbgl/text/collision_tile.hpp>
#include <mbgl/tile/tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/layout/symbol_layout.hpp>
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

#include <unordered_set>

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
                                           const PlacementConfig& config) {
    tileData = std::move(tileData_);
    return redoLayout(std::move(layers_), config);
}

TileParseResult TileWorker::redoLayout(std::vector<std::unique_ptr<Layer>> layers_,
                                       const PlacementConfig& config) {
    layers = std::move(layers_);

    // We're doing a fresh parse of the tile, because the underlying data or style has changed.
    featureIndex = std::make_unique<FeatureIndex>();
    symbolLayouts.clear();

    // We're storing a set of bucket names we've parsed to avoid parsing a bucket twice that is
    // referenced from more than one layer
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;
    std::unordered_set<std::string> parsed;

    for (auto i = layers.rbegin(); i != layers.rend(); i++) {
        if (obsolete) {
            break;
        }

        // Temporary prevention for crashing due to https://github.com/mapbox/mapbox-gl-native/issues/6263.
        // Instead, the race condition will produce a blank tile.
        if (!tileData) {
            break;
        }

        const Layer* layer = i->get();
        const std::string& bucketName = layer->baseImpl->bucketName();

        featureIndex->addBucketLayerName(bucketName, layer->baseImpl->id);

        if (parsed.find(bucketName) != parsed.end()) {
            continue;
        }

        parsed.emplace(bucketName);

        auto geometryLayer = tileData->getLayer(layer->baseImpl->sourceLayer);
        if (!geometryLayer) {
            continue;
        }

        BucketParameters parameters(id,
                                    *geometryLayer,
                                    obsolete,
                                    reinterpret_cast<uintptr_t>(this),
                                    spriteStore,
                                    glyphAtlas,
                                    glyphStore,
                                    *featureIndex,
                                    mode);

        if (layer->is<SymbolLayer>()) {
            symbolLayouts.push_back(layer->as<SymbolLayer>()->impl->createLayout(parameters));
        } else {
            std::unique_ptr<Bucket> bucket = layer->baseImpl->createBucket(parameters);
            if (bucket->hasData()) {
                buckets.emplace(layer->baseImpl->bucketName(), std::move(bucket));
            }
        }
    }

    return parsePendingLayers(config, std::move(buckets));
}

TileParseResult TileWorker::parsePendingLayers(const PlacementConfig& config) {
    return parsePendingLayers(config, std::unordered_map<std::string, std::unique_ptr<Bucket>>());
}

TileParseResult TileWorker::parsePendingLayers(const PlacementConfig& config,
                                               std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets) {
    TileParseResultData result;

    result.complete = true;
    result.buckets = std::move(buckets);

    // Prepare as many SymbolLayouts as possible.
    for (auto& symbolLayout : symbolLayouts) {
        if (symbolLayout->state == SymbolLayout::Pending) {
            if (symbolLayout->canPrepare(glyphStore, spriteStore)) {
                symbolLayout->state = SymbolLayout::Prepared;
                symbolLayout->prepare(reinterpret_cast<uintptr_t>(this),
                                      glyphAtlas,
                                      glyphStore);
            } else {
                result.complete = false;
            }
        }
    }

    // If all SymbolLayouts are prepared, then perform placement. Otherwise, parsePendingLayers
    // will eventually be re-run.
    if (result.complete) {
        TilePlacementResult placementResult = redoPlacement(config);

        featureIndex->setCollisionTile(std::move(placementResult.collisionTile));

        for (auto& bucket : placementResult.buckets) {
            result.buckets.emplace(std::move(bucket));
        }

        result.featureIndex = std::move(featureIndex);

        if (tileData) {
            result.tileData = tileData->clone();
        }
    }

    return std::move(result);
}

TilePlacementResult TileWorker::redoPlacement(const PlacementConfig& config) {
    TilePlacementResult result;

    result.collisionTile = std::make_unique<CollisionTile>(config);

    for (auto& symbolLayout : symbolLayouts) {
        if (symbolLayout->state == SymbolLayout::Pending) {
            // Can't do placement until all layouts are prepared.
            return result;
        }
    }

    for (auto& symbolLayout : symbolLayouts) {
        symbolLayout->state = SymbolLayout::Placed;
        std::unique_ptr<Bucket> bucket = symbolLayout->place(*result.collisionTile);
        if (bucket->hasData() || symbolLayout->hasSymbolInstances()) {
            result.buckets.emplace(symbolLayout->bucketName, std::move(bucket));
        }
    }

    return result;
}

} // namespace mbgl
