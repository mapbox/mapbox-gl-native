#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

TileLayerIndex::TileLayerIndex(OverscaledTileID coord_,
                               std::vector<SymbolInstance>& symbolInstances,
                               uint32_t bucketInstanceId_,
                               std::string bucketLeaderId_)
    : coord(coord_), bucketInstanceId(bucketInstanceId_), bucketLeaderId(std::move(bucketLeaderId_)) {
    for (SymbolInstance& symbolInstance : symbolInstances) {
        if (symbolInstance.crossTileID == SymbolInstance::invalidCrossTileID()) continue;
        indexedSymbolInstances[symbolInstance.key].emplace_back(symbolInstance.crossTileID,
                                                                getScaledCoordinates(symbolInstance, coord));
    }
}

Point<int64_t> TileLayerIndex::getScaledCoordinates(SymbolInstance& symbolInstance,
                                                    const OverscaledTileID& childTileCoord) const {
    // Round anchor positions to roughly 4 pixel grid
    const double roundingFactor = 512.0 / util::EXTENT / 2.0;
    const double scale = roundingFactor / std::pow(2, childTileCoord.canonical.z - coord.canonical.z);
    return {
        static_cast<int64_t>(std::floor((childTileCoord.canonical.x * util::EXTENT + symbolInstance.anchor.point.x) * scale)),
        static_cast<int64_t>(std::floor((childTileCoord.canonical.y * util::EXTENT + symbolInstance.anchor.point.y) * scale))
    };
}

void TileLayerIndex::findMatches(SymbolBucket& bucket,
                                 const OverscaledTileID& newCoord,
                                 std::set<uint32_t>& zoomCrossTileIDs) const {
    auto& symbolInstances = bucket.symbolInstances;
    float tolerance = coord.canonical.z < newCoord.canonical.z ? 1 : std::pow(2, coord.canonical.z - newCoord.canonical.z);

    if (bucket.bucketLeaderID != bucketLeaderId) return;

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

        for (const IndexedSymbolInstance& thisTileSymbol : it->second) {
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

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex(uint32_t& maxCrossTileID_) : maxCrossTileID(maxCrossTileID_) {}

/*
 * Sometimes when a user pans across the antimeridian the longitude value gets wrapped.
 * To prevent labels from flashing out and in we adjust the tileID values in the indexes
 * so that they match the new wrapped version of the map.
 */
void CrossTileSymbolLayerIndex::handleWrapJump(float newLng) {
    const int wrapDelta = std::round((newLng - lng) / 360);
    if (wrapDelta != 0) {
        std::map<uint8_t, std::map<OverscaledTileID,TileLayerIndex>> newIndexes;
        for (auto& zoomIndex : indexes) {
            std::map<OverscaledTileID,TileLayerIndex> newZoomIndex;
            for (auto& index : zoomIndex.second) {
                // change the tileID's wrap and move its index
                index.second.coord = index.second.coord.unwrapTo(index.second.coord.wrap + wrapDelta);
                newZoomIndex.emplace(index.second.coord, std::move(index.second));
            }
            newIndexes.emplace(zoomIndex.first, std::move(newZoomIndex));
        }

        indexes = std::move(newIndexes);
    }

    lng = newLng;
}

namespace {

bool isInVewport(const mat4& posMatrix, const Point<float>& point) {
    vec4 p = {{point.x, point.y, 0, 1}};
    matrix::transformMat4(p, p, posMatrix);

    // buffer covers area of the next zoom level (current zoom - 1 covered area).
    constexpr float buffer = 1.0f;
    constexpr float edge = 1.0f + buffer;
    float x = p[0] / p[3];
    float y = p[1] / p[3];
    return (x > -edge && y > -edge && x < edge && y < edge);
}

} // namespace

bool CrossTileSymbolLayerIndex::addBucket(const OverscaledTileID& tileID,
                                          const mat4& tileMatrix,
                                          SymbolBucket& bucket) {
    auto& thisZoomIndexes = indexes[tileID.overscaledZ];
    auto previousIndex = thisZoomIndexes.find(tileID);
    if (previousIndex != thisZoomIndexes.end()) {
        if (previousIndex->second.bucketInstanceId == bucket.bucketInstanceId && !bucket.hasUninitializedSymbols) {
            return false;
        } else {
            // We're replacing this bucket with an updated version
            // Remove the old bucket's "used crossTileIDs" now so that the new bucket can claim them.
            // We have to keep the old index entries themselves until the end of 'addBucket' so
            // that we can copy them with 'findMatches'.
            removeBucketCrossTileIDs(tileID.overscaledZ, previousIndex->second);
        }
    }

    bucket.hasUninitializedSymbols = false;

    if (tileID.overscaleFactor() > 1u) {
        // For overscaled tiles the viewport might be showing only a small part of the tile,
        // so we filter out the off-screen symbols to improve the performance.
        for (auto& symbolInstance : bucket.symbolInstances) {
            if (isInVewport(tileMatrix, symbolInstance.anchor.point)) {
                symbolInstance.crossTileID = 0u;
            } else {
                symbolInstance.crossTileID = SymbolInstance::invalidCrossTileID();
                bucket.hasUninitializedSymbols = true;
            }
        }
    } else {
        for (auto& symbolInstance : bucket.symbolInstances) {
            symbolInstance.crossTileID = 0u;
        }
    }

    auto& thisZoomUsedCrossTileIDs = usedCrossTileIDs[tileID.overscaledZ];

    for (auto& it : indexes) {
        auto zoom = it.first;
        const auto& zoomIndexes = it.second;
        if (zoom > tileID.overscaledZ) {
            for (auto& childIndex : zoomIndexes) {
                if (childIndex.second.coord.isChildOf(tileID)) {
                    childIndex.second.findMatches(bucket, tileID, thisZoomUsedCrossTileIDs);
                }
            }
        } else {
            auto parentTileID = tileID.scaledTo(zoom);
            auto parentIndex = zoomIndexes.find(parentTileID);
            if (parentIndex != zoomIndexes.end()) {
                parentIndex->second.findMatches(bucket, tileID, thisZoomUsedCrossTileIDs);
            }
        }
    }

    for (auto& symbolInstance : bucket.symbolInstances) {
        if (!symbolInstance.crossTileID) {
            // symbol did not match any known symbol, assign a new id
            symbolInstance.crossTileID = ++maxCrossTileID;
            thisZoomUsedCrossTileIDs.insert(symbolInstance.crossTileID);
        }
    }

    thisZoomIndexes.erase(tileID);
    thisZoomIndexes.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(tileID),
        std::forward_as_tuple(tileID, bucket.symbolInstances, bucket.bucketInstanceId, bucket.bucketLeaderID));
    return true;
}

void CrossTileSymbolLayerIndex::removeBucketCrossTileIDs(uint8_t zoom, const TileLayerIndex& removedBucket) {
    for (const auto& key : removedBucket.indexedSymbolInstances) {
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

CrossTileSymbolIndex::CrossTileSymbolIndex() = default;

auto CrossTileSymbolIndex::addLayer(const RenderLayer& layer, float lng) -> AddLayerResult {
    auto found = layerIndexes.find(layer.getID());
    if (found == layerIndexes.end()) {
        found = layerIndexes
                    .emplace(std::piecewise_construct,
                             std::forward_as_tuple(layer.getID()),
                             std::forward_as_tuple(maxCrossTileID))
                    .first;
    }
    auto& layerIndex = found->second;

    AddLayerResult result = AddLayerResult::NoChanges;
    std::unordered_set<uint32_t> currentBucketIDs;

    layerIndex.handleWrapJump(lng);

    for (const auto& item : layer.getPlacementData()) {
        const RenderTile& renderTile = item.tile;
        Bucket& bucket = item.bucket;
        auto pair = bucket.registerAtCrossTileIndex(layerIndex, renderTile);
        assert(pair.first != 0u);
        if (pair.second) result |= AddLayerResult::BucketsAdded;
        currentBucketIDs.insert(pair.first);
    }

    if (layerIndex.removeStaleBuckets(currentBucketIDs)) result |= AddLayerResult::BucketsRemoved;

    return result;
}

void CrossTileSymbolIndex::pruneUnusedLayers(const std::set<std::string>& usedLayers) {
    for (auto it = layerIndexes.begin(); it != layerIndexes.end();) {
        if (usedLayers.find(it->first) == usedLayers.end()) {
            it = layerIndexes.erase(it);
        } else {
            ++it;
        }
    }
}

void CrossTileSymbolIndex::reset() {
    layerIndexes.clear();
}

} // namespace mbgl

