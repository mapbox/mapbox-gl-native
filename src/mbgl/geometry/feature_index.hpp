#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/feature.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

class RenderedQueryOptions;
class RenderLayer;

class CollisionIndex;

class IndexedSubfeature {
public:
    IndexedSubfeature() = delete;
    IndexedSubfeature(std::size_t index_, std::string sourceLayerName_, std::string bucketName_, size_t sortIndex_)
        : index(index_)
        , sourceLayerName(std::move(sourceLayerName_))
        , bucketName(std::move(bucketName_))
        , sortIndex(sortIndex_)
        , tileID(0, 0, 0)
    {}
    
    IndexedSubfeature(std::size_t index_, std::string sourceLayerName_, std::string bucketName_, size_t sortIndex_,
                      std::string sourceID_, CanonicalTileID tileID_)
        : index(index_)
        , sourceLayerName(std::move(sourceLayerName_))
        , bucketName(std::move(bucketName_))
        , sortIndex(std::move(sortIndex_))
        , sourceID(std::move(sourceID_))
        , tileID(std::move(tileID_))
    {}
    
    size_t index;
    std::string sourceLayerName;
    std::string bucketName;
    size_t sortIndex;

    // Only used for symbol features
    std::string sourceID;
    CanonicalTileID tileID;
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
            const UnwrappedTileID&,
            const std::string&,
            const std::vector<const RenderLayer*>&,
            const CollisionIndex&,
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
