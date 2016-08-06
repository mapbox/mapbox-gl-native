#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/feature.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

namespace style {
class Style;
} // namespace style

class CollisionTile;
class CanonicalTileID;

class IndexedSubfeature {
public:
    IndexedSubfeature() = delete;
    std::size_t index;
    std::string sourceLayerName;
    std::string bucketName;
    size_t sortIndex;
};

class FeatureIndex {
public:
    FeatureIndex();

    void insert(const GeometryCollection&, std::size_t index, const std::string& sourceLayerName, const std::string& bucketName);

    void query(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCollection& queryGeometry,
            const float bearing,
            const double tileSize,
            const double scale,
            const optional<std::vector<std::string>>& layerIDs,
            const GeometryTileData&,
            const CanonicalTileID&,
            const style::Style&) const;

    static optional<GeometryCollection> translateQueryGeometry(
            const GeometryCollection& queryGeometry,
            const std::array<float, 2>& translate,
            const style::TranslateAnchorType,
            const float bearing,
            const float pixelsToTileUnits);

    void addBucketLayerName(const std::string& bucketName, const std::string& layerName);

    void setCollisionTile(std::unique_ptr<CollisionTile>);

private:
    void addFeature(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const IndexedSubfeature&,
            const GeometryCollection& queryGeometry,
            const optional<std::vector<std::string>>& filterLayerIDs,
            const GeometryTileData&,
            const CanonicalTileID&,
            const style::Style&,
            const float bearing,
            const float pixelsToTileUnits) const;

    std::unique_ptr<CollisionTile> collisionTile;
    GridIndex<IndexedSubfeature> grid;
    unsigned int sortIndex = 0;

    std::unordered_map<std::string, std::vector<std::string>> bucketLayerIDs;
};
} // namespace mbgl
