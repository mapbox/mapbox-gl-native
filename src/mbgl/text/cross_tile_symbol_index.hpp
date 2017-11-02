#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace mbgl {

class SymbolInstance;
class RenderSymbolLayer;
class SymbolBucket;

class IndexEntry {
    Point<float> anchorPoint;

};

class IndexedSymbolInstance {
    public:
        IndexedSymbolInstance(uint32_t crossTileID_, Point<double> coord_)
            : crossTileID(crossTileID_), coord(coord_) {};
        uint32_t crossTileID;
        Point<double> coord;
};

class TileLayerIndex {
    public:
        TileLayerIndex(OverscaledTileID coord, std::vector<SymbolInstance>&);

        Point<double> getScaledCoordinates(SymbolInstance&, const OverscaledTileID&);
        void findMatches(std::vector<SymbolInstance>&, const OverscaledTileID&);
        
        OverscaledTileID coord;
        std::map<std::u16string,std::vector<IndexedSymbolInstance>> indexedSymbolInstances;
};

class CrossTileSymbolLayerIndex {
    public:
        CrossTileSymbolLayerIndex();
        void addBucket(const OverscaledTileID&, SymbolBucket&);
    private:
        std::map<uint8_t,std::map<OverscaledTileID,TileLayerIndex>> indexes;
        uint32_t maxBucketInstanceId = 0;
        static uint32_t maxCrossTileID;
};

class CrossTileSymbolIndex {
    public:
        CrossTileSymbolIndex();

        void addLayer(RenderSymbolLayer&);
    private:
        std::map<std::string,CrossTileSymbolLayerIndex> layerIndexes;
};

} // namespace mbgl
