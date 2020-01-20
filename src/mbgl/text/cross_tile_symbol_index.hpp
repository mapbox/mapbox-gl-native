#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/bitmask_operations.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/optional.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

namespace mbgl {

class SymbolInstance;
class RenderLayer;
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
    TileLayerIndex(OverscaledTileID coord,
                   std::vector<SymbolInstance>&,
                   uint32_t bucketInstanceId,
                   std::string bucketLeaderId);

    Point<int64_t> getScaledCoordinates(SymbolInstance&, const OverscaledTileID&) const;
    void findMatches(SymbolBucket&, const OverscaledTileID&, std::set<uint32_t>&) const;

    OverscaledTileID coord;
    uint32_t bucketInstanceId;
    std::string bucketLeaderId;
    std::map<std::u16string,std::vector<IndexedSymbolInstance>> indexedSymbolInstances;
};

class CrossTileSymbolLayerIndex {
public:
    CrossTileSymbolLayerIndex(uint32_t& maxCrossTileID);
    bool addBucket(const OverscaledTileID&, const mat4& tileMatrix, SymbolBucket&);
    bool removeStaleBuckets(const std::unordered_set<uint32_t>& currentIDs);
    void handleWrapJump(float newLng);
private:
    void removeBucketCrossTileIDs(uint8_t zoom, const TileLayerIndex& removedBucket);

    std::map<uint8_t, std::map<OverscaledTileID,TileLayerIndex>> indexes;
    std::map<uint8_t, std::set<uint32_t>> usedCrossTileIDs;
    float lng = 0;
    uint32_t& maxCrossTileID;
};

class CrossTileSymbolIndex {
public:
    CrossTileSymbolIndex();

    enum class AddLayerResult : uint8_t {
        NoChanges = 0,
        BucketsAdded = 1 << 0,
        BucketsRemoved = 1 << 1
    };

    AddLayerResult addLayer(const RenderLayer& layer, float lng);
    void pruneUnusedLayers(const std::set<std::string>&);

    void reset();
private:
    std::map<std::string, CrossTileSymbolLayerIndex> layerIndexes;
    uint32_t maxCrossTileID = 0;
};

} // namespace mbgl
