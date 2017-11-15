#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace mbgl {

class SymbolInstance;
class RenderSymbolLayer;
class SymbolBucket;

class IndexedSymbolInstance {
public:
    IndexedSymbolInstance(uint32_t crossTileID_, Point<int64_t> coord_)
        : crossTileID(crossTileID_), coord(coord_)
    {}

    uint32_t crossTileID;
    Point<int64_t> coord;
};

class TileLayerIndex {
public:
    TileLayerIndex(OverscaledTileID coord, std::vector<SymbolInstance>&, uint32_t bucketInstanceId);

    Point<int64_t> getScaledCoordinates(SymbolInstance&, const OverscaledTileID&);
    void findMatches(std::vector<SymbolInstance>&, const OverscaledTileID&);
    
    OverscaledTileID coord;
    uint32_t bucketInstanceId;
    std::map<std::u16string,std::vector<IndexedSymbolInstance>> indexedSymbolInstances;
};

class CrossTileSymbolLayerIndex {
public:
    CrossTileSymbolLayerIndex();
    void addBucket(const OverscaledTileID&, SymbolBucket&, uint32_t& maxCrossTileID);
    bool removeStaleBuckets(const std::unordered_set<uint32_t>& currentIDs);
private:
    std::map<uint8_t,std::map<OverscaledTileID,TileLayerIndex>> indexes;
    uint32_t maxBucketInstanceId = 0;
};

class CrossTileSymbolIndex {
public:
    CrossTileSymbolIndex();

    bool addLayer(RenderSymbolLayer&);

    void reset();
private:
    std::map<std::string, CrossTileSymbolLayerIndex> layerIndexes;
    uint32_t maxCrossTileID = 0;
};

} // namespace mbgl
