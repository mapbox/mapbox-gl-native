#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/mat4.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

class RenderedQueryOptions;
class RenderLayer;
class TransformState;
class SourceFeatureState;

class CollisionIndex;

class IndexedSubfeature {
public:
    IndexedSubfeature() = delete;
    IndexedSubfeature(std::size_t index_, std::string sourceLayerName_, std::string bucketName_, size_t sortIndex_)
        : index(index_)
        , sourceLayerName(std::move(sourceLayerName_))
        , bucketLeaderID(std::move(bucketName_))
        , sortIndex(sortIndex_)
        , bucketInstanceId(0)
        , collisionGroupId(0)
    {}
    
    
    IndexedSubfeature(const IndexedSubfeature& other, uint32_t bucketInstanceId_, uint16_t collisionGroupId_)
        : index(other.index)
        , sourceLayerName(other.sourceLayerName)
        , bucketLeaderID(other.bucketLeaderID)
        , sortIndex(other.sortIndex)
        , bucketInstanceId(bucketInstanceId_)
        , collisionGroupId(collisionGroupId_)
    {}
    size_t index;
    std::string sourceLayerName;
    std::string bucketLeaderID;
    size_t sortIndex;

    // Only used for symbol features
    uint32_t bucketInstanceId;
    uint16_t collisionGroupId;
};

using FeatureSortOrder = std::shared_ptr<const std::vector<size_t>>;

class FeatureIndex {
public:
    FeatureIndex(std::unique_ptr<const GeometryTileData> tileData_);

    const GeometryTileData* getData() { return tileData.get(); }
    
    void insert(const GeometryCollection&, std::size_t index, const std::string& sourceLayerName, const std::string& bucketLeaderID);

    void query(std::unordered_map<std::string, std::vector<Feature>>& result, const GeometryCoordinates& queryGeometry,
               const TransformState&, const mat4& posMatrix, const double tileSize, const double scale,
               const RenderedQueryOptions& options, const UnwrappedTileID&,
               const std::unordered_map<std::string, const RenderLayer*>&, const float additionalQueryPadding,
               const SourceFeatureState& sourceFeatureState) const;

    static optional<GeometryCoordinates> translateQueryGeometry(
            const GeometryCoordinates& queryGeometry,
            const std::array<float, 2>& translate,
            const style::TranslateAnchorType,
            const float bearing,
            const float pixelsToTileUnits);

    void setBucketLayerIDs(const std::string& bucketLeaderID, const std::vector<std::string>& layerIDs);

    std::unordered_map<std::string, std::vector<Feature>> lookupSymbolFeatures(
        const std::vector<IndexedSubfeature>& symbolFeatures,
        const RenderedQueryOptions& options,
        const std::unordered_map<std::string, const RenderLayer*>& layers,
        const OverscaledTileID& tileID,
        const FeatureSortOrder& featureSortOrder) const;

private:
    void addFeature(std::unordered_map<std::string, std::vector<Feature>>& result, const IndexedSubfeature&,
                    const RenderedQueryOptions& options, const CanonicalTileID&,
                    const std::unordered_map<std::string, const RenderLayer*>&,
                    const GeometryCoordinates& queryGeometry, const TransformState& transformState,
                    const float pixelsToTileUnits, const mat4& posMatrix,
                    const SourceFeatureState* sourceFeatureState) const;

    GridIndex<IndexedSubfeature> grid;
    unsigned int sortIndex = 0;

    std::unordered_map<std::string, std::vector<std::string>> bucketLayerIDs;
    std::unique_ptr<const GeometryTileData> tileData;
};
} // namespace mbgl
