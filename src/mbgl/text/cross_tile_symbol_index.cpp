#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {


TileLayerIndex::TileLayerIndex(OverscaledTileID coord_, std::vector<SymbolInstance>& symbolInstances, uint32_t bucketInstanceId_)
    : coord(coord_), bucketInstanceId(bucketInstanceId_) {
        for (SymbolInstance& symbolInstance : symbolInstances) {
            indexedSymbolInstances[symbolInstance.key].emplace_back(symbolInstance.crossTileID, getScaledCoordinates(symbolInstance, coord));
        }
    }

Point<int64_t> TileLayerIndex::getScaledCoordinates(SymbolInstance& symbolInstance, const OverscaledTileID& childTileCoord) {
    // Round anchor positions to roughly 4 pixel grid
    const double roundingFactor = 512.0 / util::EXTENT / 2.0;
    const double scale = roundingFactor / std::pow(2, childTileCoord.canonical.z - coord.canonical.z);
    return {
        static_cast<int64_t>(std::floor((childTileCoord.canonical.x * util::EXTENT + symbolInstance.anchor.point.x) * scale)),
        static_cast<int64_t>(std::floor((childTileCoord.canonical.y * util::EXTENT + symbolInstance.anchor.point.y) * scale))
    };
}

void TileLayerIndex::findMatches(std::vector<SymbolInstance>& symbolInstances, const OverscaledTileID& newCoord) {
    float tolerance = coord.canonical.z < newCoord.canonical.z ? 1 : std::pow(2, coord.canonical.z - newCoord.canonical.z);

    for (auto& symbolInstance : symbolInstances) {
        if (symbolInstance.crossTileID) {
            // already has a match, skip
            continue;
        }

        auto it = indexedSymbolInstances.find(symbolInstance.key);
        if (it == indexedSymbolInstances.end()) {
            // No symbol with this key in this bucket
            continue;
        }

        auto scaledSymbolCoord = getScaledCoordinates(symbolInstance, newCoord);

        for (IndexedSymbolInstance& thisTileSymbol: it->second) {
            // Return any symbol with the same keys whose coordinates are within 1
            // grid unit. (with a 4px grid, this covers a 12px by 12px area)
            if (std::abs(thisTileSymbol.coord.x - scaledSymbolCoord.x) <= tolerance &&
                std::abs(thisTileSymbol.coord.y - scaledSymbolCoord.y) <= tolerance) {

                symbolInstance.crossTileID = thisTileSymbol.crossTileID;
                break;
            }
        }
    }
}

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex() {
}

void CrossTileSymbolLayerIndex::addBucket(const OverscaledTileID& coord, SymbolBucket& bucket, uint32_t& maxCrossTileID) {
    if (bucket.bucketInstanceId) return;
    bucket.bucketInstanceId = ++maxBucketInstanceId;

    uint8_t minZoom = 25;
    uint8_t maxZoom = 0;
    for (auto& it : indexes) {
        auto z = it.first;
        minZoom = std::min(minZoom, z);
        maxZoom = std::max(maxZoom, z);
    }


    // make all higher-res child tiles block duplicate labels in this tile
    for (auto z = maxZoom; z > coord.overscaledZ; z--) {
        auto zoomIndexes = indexes.find(z);
        if (zoomIndexes != indexes.end()) {
            for (auto& childIndex : zoomIndexes->second) {
                if (!childIndex.second.coord.isChildOf(coord)) {
                    continue;
                }
                childIndex.second.findMatches(bucket.symbolInstances, coord);
            }
        }
        if (z == 0) {
            break;
        }
    }

    // make this tile block duplicate labels in lower-res parent tiles
    for (auto z = coord.overscaledZ; z >= minZoom; z--) {
        auto parentCoord = coord.scaledTo(z);
        auto zoomIndexes = indexes.find(z);
        if (zoomIndexes != indexes.end()) {
            auto parentIndex = zoomIndexes->second.find(parentCoord);
            if (parentIndex != zoomIndexes->second.end()) {
                parentIndex->second.findMatches(bucket.symbolInstances, coord);
            }
        }
        if (z == 0) {
            break;
        }
    }
    
    for (auto& symbolInstance : bucket.symbolInstances) {
        if (!symbolInstance.crossTileID) {
            // symbol did not match any known symbol, assign a new id
            symbolInstance.crossTileID = ++maxCrossTileID;
        }
    }

    indexes[coord.overscaledZ].emplace(coord, TileLayerIndex(coord, bucket.symbolInstances, bucket.bucketInstanceId));
}

bool CrossTileSymbolLayerIndex::removeStaleBuckets(const std::unordered_set<uint32_t>& currentIDs) {
    bool tilesChanged = false;
    for (auto& zoomIndexes : indexes) {
        for (auto it = zoomIndexes.second.begin(); it != zoomIndexes.second.end();) {
            if (!currentIDs.count(it->second.bucketInstanceId)) {
                it = zoomIndexes.second.erase(it);
                tilesChanged = true;
            } else {
                ++it;
            }
        }
    }
    return tilesChanged;
}

CrossTileSymbolIndex::CrossTileSymbolIndex() {}

bool CrossTileSymbolIndex::addLayer(RenderSymbolLayer& symbolLayer) {

    auto& layerIndex = layerIndexes[symbolLayer.getID()];

    bool symbolBucketsChanged = false;
    std::unordered_set<uint32_t> currentBucketIDs;

    for (RenderTile& renderTile : symbolLayer.renderTiles) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);

        if (!symbolBucket.bucketInstanceId) {
            symbolBucketsChanged = true;
        }
        layerIndex.addBucket(renderTile.tile.id, symbolBucket, maxCrossTileID);
        currentBucketIDs.insert(symbolBucket.bucketInstanceId);
    }

    if (layerIndex.removeStaleBuckets(currentBucketIDs)) {
        symbolBucketsChanged = true;
    }
    return symbolBucketsChanged;
}

void CrossTileSymbolIndex::reset() {
    layerIndexes.clear();
}

} // namespace mbgl

