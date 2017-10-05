#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>

namespace mbgl {


TileLayerIndex::TileLayerIndex(OverscaledTileID coord, std::shared_ptr<std::vector<SymbolInstance>> symbolInstances)
    : coord(coord), symbolInstances(symbolInstances) {
        for (SymbolInstance& symbolInstance : *symbolInstances) {
            if (indexedSymbolInstances.find(symbolInstance.key) == indexedSymbolInstances.end()) {
                indexedSymbolInstances.emplace(symbolInstance.key, std::vector<IndexedSymbolInstance>());
            }

            indexedSymbolInstances.at(symbolInstance.key).emplace_back(symbolInstance, getScaledCoordinates(symbolInstance, coord));

            symbolInstance.isDuplicate = false;
            // symbolInstance.isDuplicate = false;
            // If we don't pick up an opacity from our parent or child tiles
            // Reset so that symbols in cached tiles fade in the same
            // way as freshly loaded tiles
            //symbolInstance.textOpacityState = OpacityState();
            //symbolInstance.iconOpacityState = OpacityState();
        }
    }

Point<double> TileLayerIndex::getScaledCoordinates(SymbolInstance& symbolInstance, OverscaledTileID& childTileCoord) {
    // Round anchor positions to roughly 4 pixel grid
    const double roundingFactor = 512.0 / util::EXTENT / 2.0;
    const double scale = roundingFactor / std::pow(2, childTileCoord.canonical.z - coord.canonical.z);
    return {
        std::floor((childTileCoord.canonical.x * util::EXTENT + symbolInstance.anchor.point.x) * scale),
        std::floor((childTileCoord.canonical.y * util::EXTENT + symbolInstance.anchor.point.y) * scale)
    };
}

optional<SymbolInstance> TileLayerIndex::getMatchingSymbol(SymbolInstance& childTileSymbol, OverscaledTileID& childTileCoord) {
    auto it = indexedSymbolInstances.find(childTileSymbol.key);
    if (it == indexedSymbolInstances.end()) return {};

    Point<double> childTileSymbolCoord = getScaledCoordinates(childTileSymbol, childTileCoord);

    for (IndexedSymbolInstance& thisTileSymbol: it->second) {
		// Return any symbol with the same keys whose coordinates are within 1
        // grid unit. (with a 4px grid, this covers a 12px by 12px area)
        if (std::fabs(thisTileSymbol.coord.x - childTileSymbolCoord.x) <= 1 &&
            std::fabs(thisTileSymbol.coord.y - childTileSymbolCoord.y) <= 1) {
            return { thisTileSymbol.instance };
        }
    }

    return {};
}

CrossTileSymbolLayerIndex::CrossTileSymbolLayerIndex() {
}

void CrossTileSymbolLayerIndex::addTile(const OverscaledTileID& coord, std::shared_ptr<std::vector<SymbolInstance>> symbolInstances) {
    uint8_t minZoom = 25;
    uint8_t maxZoom = 0;
    for (auto& it : indexes) {
        auto z = it.first;
        minZoom = std::min(minZoom, z);
        maxZoom = std::max(maxZoom, z);
    }

    TileLayerIndex tileIndex(coord, symbolInstances);

    // make all higher-res child tiles block duplicate labels in this tile
    for (auto z = maxZoom; z > coord.overscaledZ; z--) {
        auto zoomIndexes = indexes.find(coord.overscaledZ);
        if (zoomIndexes != indexes.end()) {
            for (auto& childIndex : zoomIndexes->second) {
                if (!childIndex.second.coord.isChildOf(coord)) continue;
                // Mark labels in this tile blocked, and don't copy opacity state
                // into this tile
                blockLabels(childIndex.second, tileIndex, false);
            }
        }
    }

    // make this tile block duplicate labels in lower-res parent tiles
    for (auto z = coord.overscaledZ - 1; z >= minZoom; z--) {
        auto parentCoord = coord.scaledTo(z);
        auto zoomIndexes = indexes.find(z);
        if (zoomIndexes != indexes.end()) {
            auto parentIndex = zoomIndexes->second.find(parentCoord);
            if (parentIndex != zoomIndexes->second.end()) {
                // Mark labels in the parent tile blocked, and copy opacity state
                // into this tile
                blockLabels(tileIndex, parentIndex->second, true);
            }
        }
    }
    
    if (indexes.find(coord.overscaledZ) == indexes.end()) {
        indexes.emplace(coord.overscaledZ, std::map<OverscaledTileID,TileLayerIndex>());
    }

    indexes.at(coord.overscaledZ).emplace(coord, std::move(tileIndex));
}

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

void CrossTileSymbolLayerIndex::blockLabels(TileLayerIndex& childIndex, TileLayerIndex& parentIndex, bool copyParentOpacity) {
    for (auto& symbolInstance : *childIndex.symbolInstances) {
        // only non-duplicate labels can block other labels
        if (!symbolInstance.isDuplicate) {
            auto parentSymbolInstance = parentIndex.getMatchingSymbol(symbolInstance, childIndex.coord);
            if (parentSymbolInstance) {
                // if the parent label was previously non-duplicate, make it duplicate because it's now blocked
                if (!parentSymbolInstance->isDuplicate) {
                    parentSymbolInstance->isDuplicate = true;

                    // If the child label is the one being added to the index,
                    // copy the parent's opacity to the child
                    if (copyParentOpacity) {
                        //symbolInstance.textOpacityState = parentSymbolInstance->textOpacityState;
                        //symbolInstance.iconOpacityState = parentSymbolInstance->iconOpacityState;
                    }
                }
            }
        }
    }
}

void CrossTileSymbolLayerIndex::unblockLabels(TileLayerIndex& childIndex, TileLayerIndex& parentIndex) {
    assert(childIndex.coord.overscaledZ > parentIndex.coord.overscaledZ);
    for (auto& symbolInstance : *childIndex.symbolInstances) {
        // only non-duplicate labels were blocking other labels
        if (!symbolInstance.isDuplicate) {
            auto parentSymbolInstance = parentIndex.getMatchingSymbol(symbolInstance, childIndex.coord);
            if (parentSymbolInstance) {
                // this label is now unblocked, copy its opacity state
                parentSymbolInstance->isDuplicate = false;
                //parentSymbolInstance->textOpacityState = symbolInstance.textOpacityState;
                //parentSymbolInstance->iconOpacityState = symbolInstance.iconOpacityState;

                // mark child as duplicate so that it doesn't unblock further tiles at lower res
                // in the remaining calls to unblockLabels before it's fully removed
                symbolInstance.isDuplicate = true;
            }
        }
    }
}

CrossTileSymbolIndex::CrossTileSymbolIndex() {}

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
} // namespace mbgl
