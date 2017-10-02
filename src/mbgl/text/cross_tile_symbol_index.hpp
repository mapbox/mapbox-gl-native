#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>

#include <map>
#include <vector>
#include <string>

namespace mbgl {

class SymbolInstance;

class IndexedSymbolInstance {
    public:
        IndexedSymbolInstance(SymbolInstance& symbolInstance, Point<double> coord)
            : instance(symbolInstance), coord(coord) {};
        SymbolInstance& instance;
        Point<double> coord;
};

class TileLayerIndex {
    public:
        TileLayerIndex(OverscaledTileID coord, std::shared_ptr<std::vector<SymbolInstance>>);

        Point<double> getScaledCoordinates(SymbolInstance&, OverscaledTileID&);
        optional<SymbolInstance> getMatchingSymbol(SymbolInstance& childTileSymbol, OverscaledTileID& childTileCoord);
        
        OverscaledTileID coord;
        std::map<std::u16string,std::vector<IndexedSymbolInstance>> indexedSymbolInstances;
        std::shared_ptr<std::vector<SymbolInstance>> symbolInstances;
};

class CrossTileSymbolLayerIndex {
    public:
        CrossTileSymbolLayerIndex();

        void addTile(const OverscaledTileID&, std::shared_ptr<std::vector<SymbolInstance>>);
        void removeTile(const OverscaledTileID&);
        void blockLabels(TileLayerIndex& childIndex, TileLayerIndex& parentIndex, bool copyParentOpacity);
        void unblockLabels(TileLayerIndex& childIndex, TileLayerIndex& parentIndex);
    private:
        std::map<uint8_t,std::map<OverscaledTileID,TileLayerIndex>> indexes;
};

class CrossTileSymbolIndex {
    public:
        CrossTileSymbolIndex();

        void addTileLayer(std::string& layerId, const OverscaledTileID&, std::shared_ptr<std::vector<SymbolInstance>>);
        void removeTileLayer(std::string& layerId, const OverscaledTileID&);
    private:
        std::map<std::string,CrossTileSymbolLayerIndex> layerIndexes;
};

} // namespace mbgl
