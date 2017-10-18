#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/feature.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

class RenderedQueryOptions;
class RenderLayer;

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
            const GeometryCoordinates& queryGeometry,
            const float bearing,
            const double tileSize,
            const double scale,
            const RenderedQueryOptions& options,
            const GeometryTileData&,
            const CanonicalTileID&,
            const std::vector<const RenderLayer*>&,
            const CollisionTile*,
            const float additionalQueryRadius) const;

    static optional<GeometryCoordinates> translateQueryGeometry(
            const GeometryCoordinates& queryGeometry,
            const std::array<float, 2>& translate,
            const style::TranslateAnchorType,
            const float bearing,
            const float pixelsToTileUnits);

    void setBucketLayerIDs(const std::string& bucketName, const std::vector<std::string>& layerIDs);

private:
    void addFeature(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const IndexedSubfeature&,
            const GeometryCoordinates& queryGeometry,
            const RenderedQueryOptions& options,
            const GeometryTileData&,
            const CanonicalTileID&,
            const std::vector<const RenderLayer*>&,
            const float bearing,
            const float pixelsToTileUnits) const;

    GridIndex<IndexedSubfeature> grid;
    unsigned int sortIndex = 0;

    std::unordered_map<std::string, std::vector<std::string>> bucketLayerIDs;
};
} // namespace mbgl
