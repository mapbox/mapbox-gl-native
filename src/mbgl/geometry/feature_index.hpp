#ifndef MBGL_GEOMETRY_FEATURE_INDEX
#define MBGL_GEOMETRY_FEATURE_INDEX

#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/util/grid_index.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

class Style;
class CollisionTile;
enum class TranslateAnchorType : bool;

class IndexedSubfeature {
    public:
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
                std::unordered_map<std::string, std::vector<std::string>>& result,
                const GeometryCollection& queryGeometry,
                const float bearing,
                const double tileSize,
                const double scale,
                const optional<std::vector<std::string>>& layerIDs,
                const GeometryTile& geometryTile,
                const Style&) const;

        static optional<GeometryCollection> translateQueryGeometry(
                const GeometryCollection& queryGeometry,
                const std::array<float, 2>& translate,
                const TranslateAnchorType,
                const float bearing,
                const float pixelsToTileUnits);

        void addBucketLayerName(const std::string &bucketName, const std::string &layerName);

        void setCollisionTile(std::unique_ptr<CollisionTile>);

    private:

        void addFeature(
                std::unordered_map<std::string, std::vector<std::string>>& result,
                const IndexedSubfeature &indexedFeature,
                const GeometryCollection& queryGeometry,
                const optional<std::vector<std::string>>& filterLayerIDs,
                const GeometryTile& geometryTile,
                const Style& style,
                const float bearing,
                const float pixelsToTileUnits) const;

        std::unique_ptr<CollisionTile> collisionTile;
        GridIndex<IndexedSubfeature> grid;
        unsigned int sortIndex = 0;

        std::unordered_map<std::string,std::vector<std::string>> bucketLayerIDs;

};
}

#endif
