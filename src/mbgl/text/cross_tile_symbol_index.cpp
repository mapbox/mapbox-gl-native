#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {


TileLayerIndex::TileLayerIndex(OverscaledTileID coord_, std::vector<SymbolInstance>& symbolInstances) 
    : coord(coord_) {
        for (SymbolInstance& symbolInstance : symbolInstances) {
            if (symbolInstance.insideTileBoundaries) {
                indexedSymbolInstances[symbolInstance.key].emplace_back(symbolInstance.crossTileID, getScaledCoordinates(symbolInstance, coord));
            }
        }
    }

Point<double> TileLayerIndex::getScaledCoordinates(SymbolInstance& symbolInstance, const OverscaledTileID& childTileCoord) {
    // Round anchor positions to roughly 4 pixel grid
    const double roundingFactor = 512.0 / util::EXTENT / 2.0;
    const double scale = roundingFactor / std::pow(2, childTileCoord.canonical.z - coord.canonical.z);
    return {
        std::floor((childTileCoord.canonical.x * util::EXTENT + symbolInstance.anchor.point.x) * scale),
        std::floor((childTileCoord.canonical.y * util::EXTENT + symbolInstance.anchor.point.y) * scale)
    };
}

void TileLayerIndex::findMatches(std::vector<SymbolInstance>& symbolInstances, const OverscaledTileID& newCoord) {
    float tolerance = coord.canonical.z < newCoord.canonical.z ? 1 : std::pow(2, coord.canonical.z - newCoord.canonical.z);
    tolerance = 100;

    for (auto& symbolInstance : symbolInstances) {
        // already has a match, skip
        if (symbolInstance.crossTileID) continue;

        auto it = indexedSymbolInstances.find(symbolInstance.key);
        if (it == indexedSymbolInstances.end()) continue;

        Point<double> scaledSymbolCoord = getScaledCoordinates(symbolInstance, newCoord);

        for (IndexedSymbolInstance& thisTileSymbol: it->second) {
            // Return any symbol with the same keys whose coordinates are within 1
            // grid unit. (with a 4px grid, this covers a 12px by 12px area)
            if (std::fabs(thisTileSymbol.coord.x - scaledSymbolCoord.x) <= tolerance &&
                std::fabs(thisTileSymbol.coord.y - scaledSymbolCoord.y) <= tolerance) {

                symbolInstance.crossTileID = thisTileSymbol.crossTileID;
                break;
            }
        }
    }
}

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex() {
}

uint32_t CrossTileSymbolLayerIndex::maxCrossTileID = 0;

void CrossTileSymbolLayerIndex::addBucket(const OverscaledTileID& coord, SymbolBucket& bucket) {
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
                if (!childIndex.second.coord.isChildOf(coord)) continue;
                childIndex.second.findMatches(bucket.symbolInstances, coord);
            }
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
    }
    
    for (auto& symbolInstance : bucket.symbolInstances) {
        if (!symbolInstance.crossTileID && symbolInstance.insideTileBoundaries) {
            // symbol did not match any known symbol, assign a new id
            symbolInstance.crossTileID = ++maxCrossTileID;
        }
    }

    TileLayerIndex tileIndex(coord, bucket.symbolInstances);
    indexes[coord.overscaledZ].emplace(coord, std::move(tileIndex));
}

/*
void CrossTileSymbolLayerIndex::removeTile(const OverscaledTileID& coord) {
	
	auto removedIndex = indexes.at(coord.overscaledZ).at(coord);

    uint8_t minZoom = 25;
    for (auto& it : indexes) {
        auto z = it.first;
        minZoom = std::min(minZoom, z);
    }

    for (auto z = coord.overscaledZ - 1; z >= minZoom; z--) {
        auto parentCoord = coord.scaledTo(z);
        auto zoomIndexes = indexes.find(z);
        if (zoomIndexes != indexes.end()) {
            auto parentIndex = zoomIndexes->second.find(parentCoord);
            if (parentIndex != zoomIndexes->second.end()) {
                unblockLabels(removedIndex, parentIndex->second);
            }
        }
    }

    indexes.at(coord.overscaledZ).erase(coord);
    if (indexes.at(coord.overscaledZ).size() == 0) {
        indexes.erase(coord.overscaledZ);
    }
}
*/

CrossTileSymbolIndex::CrossTileSymbolIndex() {}

void CrossTileSymbolIndex::addLayer(RenderSymbolLayer& symbolLayer) {

    auto& layerIndex = layerIndexes[symbolLayer.getID()];

     for (RenderTile& renderTile : symbolLayer.renderTiles) {
         if (!renderTile.tile.isRenderable()) {
             continue;
         }



         auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
         assert(dynamic_cast<SymbolBucket*>(bucket));
         SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);

         layerIndex.addBucket(renderTile.tile.id, symbolBucket);
     }
}

/*
void CrossTileSymbolIndex::addTileLayer(std::string& layerId, const OverscaledTileID& coord, std::shared_ptr<std::vector<SymbolInstance>> symbolInstances) {
    if (layerIndexes.find(layerId) == layerIndexes.end()) {
        layerIndexes.emplace(layerId, CrossTileSymbolLayerIndex());
    }

    CrossTileSymbolLayerIndex& layerIndex = layerIndexes.at(layerId);
    layerIndex.addTile(coord, symbolInstances);
}

void CrossTileSymbolIndex::removeTileLayer(std::string& layerId, const OverscaledTileID& coord) {
    auto it = layerIndexes.find(layerId);
    if (it != layerIndexes.end()) {
        it->second.removeTile(coord);
    }
}
*/
} // namespace mbgl
