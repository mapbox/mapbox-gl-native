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

void TileLayerIndex::findMatches(std::vector<SymbolInstance>& symbolInstances, const OverscaledTileID& newCoord, std::set<uint32_t>& zoomCrossTileIDs) {
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
                std::abs(thisTileSymbol.coord.y - scaledSymbolCoord.y) <= tolerance &&
                zoomCrossTileIDs.find(thisTileSymbol.crossTileID) == zoomCrossTileIDs.end()) {
                // Once we've marked ourselves duplicate against this parent symbol,
                // don't let any other symbols at the same zoom level duplicate against
                // the same parent (see issue #10844)
                zoomCrossTileIDs.insert(thisTileSymbol.crossTileID);
                symbolInstance.crossTileID = thisTileSymbol.crossTileID;
                break;
            }
        }
    }
}

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex() {
}

bool CrossTileSymbolLayerIndex::addBucket(const OverscaledTileID& tileID, SymbolBucket& bucket, uint32_t& maxCrossTileID) {
    auto thisZoomIndexes = indexes[tileID.overscaledZ];
    auto previousIndex = thisZoomIndexes.find(tileID);
    if (previousIndex != thisZoomIndexes.end()) {
        if (previousIndex->second.bucketInstanceId == bucket.bucketInstanceId) {
            return false;
        } else {
            // We're replacing this bucket with an updated version
            // Remove the old bucket's "used crossTileIDs" now so that the new bucket can claim them.
            // We have to keep the old index entries themselves until the end of 'addBucket' so
            // that we can copy them with 'findMatches'.
            removeBucketCrossTileIDs(tileID.overscaledZ, previousIndex->second);
        }
    }

    for (auto& symbolInstance: bucket.symbolInstances) {
        symbolInstance.crossTileID = 0;
    }

    for (auto& it : indexes) {
        auto zoom = it.first;
        auto zoomIndexes = it.second;
        if (zoom > tileID.overscaledZ) {
            for (auto& childIndex : zoomIndexes) {
                if (childIndex.second.coord.isChildOf(tileID)) {
                    childIndex.second.findMatches(bucket.symbolInstances, tileID, usedCrossTileIDs[tileID.overscaledZ]);
                }
            }
        } else {
            auto parentTileID = tileID.scaledTo(zoom);
            auto parentIndex = zoomIndexes.find(parentTileID);
            if (parentIndex != zoomIndexes.end()) {
                parentIndex->second.findMatches(bucket.symbolInstances, tileID, usedCrossTileIDs[tileID.overscaledZ]);
            }
        }
    }

    for (auto& symbolInstance : bucket.symbolInstances) {
        if (!symbolInstance.crossTileID) {
            // symbol did not match any known symbol, assign a new id
            symbolInstance.crossTileID = ++maxCrossTileID;
            usedCrossTileIDs[tileID.overscaledZ].insert(symbolInstance.crossTileID);
        }
    }


    indexes[tileID.overscaledZ].erase(tileID);
    indexes[tileID.overscaledZ].emplace(tileID, TileLayerIndex(tileID, bucket.symbolInstances, bucket.bucketInstanceId));
    return true;
}

void CrossTileSymbolLayerIndex::removeBucketCrossTileIDs(uint8_t zoom, const TileLayerIndex& removedBucket) {
    for (auto key : removedBucket.indexedSymbolInstances) {
        for (auto indexedSymbolInstance : key.second) {
            usedCrossTileIDs[zoom].erase(indexedSymbolInstance.crossTileID);
        }
    }
}

bool CrossTileSymbolLayerIndex::removeStaleBuckets(const std::unordered_set<uint32_t>& currentIDs) {
    bool tilesChanged = false;
    for (auto& zoomIndexes : indexes) {
        for (auto it = zoomIndexes.second.begin(); it != zoomIndexes.second.end();) {
            if (!currentIDs.count(it->second.bucketInstanceId)) {
                removeBucketCrossTileIDs(zoomIndexes.first, it->second);
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
            symbolBucket.bucketInstanceId = ++maxBucketInstanceId;
        }

        const bool bucketAdded = layerIndex.addBucket(renderTile.tile.id, symbolBucket, maxCrossTileID);
        symbolBucketsChanged = symbolBucketsChanged || bucketAdded;
        currentBucketIDs.insert(symbolBucket.bucketInstanceId);
    }

    if (layerIndex.removeStaleBuckets(currentBucketIDs)) {
        symbolBucketsChanged = true;
    }
    return symbolBucketsChanged;
}

void CrossTileSymbolIndex::pruneUnusedLayers(const std::set<std::string>& usedLayers) {
    std::vector<std::string> unusedLayers;
    for (auto layerIndex : layerIndexes) {
        if (usedLayers.find(layerIndex.first) == usedLayers.end()) {
            unusedLayers.push_back(layerIndex.first);
        }
    }
    for (auto unusedLayer : unusedLayers) {
        layerIndexes.erase(unusedLayer);
    }
}

void CrossTileSymbolIndex::reset() {
    layerIndexes.clear();
}

} // namespace mbgl

