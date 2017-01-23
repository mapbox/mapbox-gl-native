#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/actor/actor.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

namespace mbgl {

class GeometryTileData;
class FeatureIndex;
class CollisionTile;

namespace style {
class Style;
class Layer;
class UpdateParameters;
} // namespace style

class GeometryTile : public Tile {
public:
    GeometryTile(const OverscaledTileID&,
                 std::string sourceID,
                 const style::UpdateParameters&);

    ~GeometryTile() override;

    void setError(std::exception_ptr);
    void setData(std::unique_ptr<const GeometryTileData>);

    void setPlacementConfig(const PlacementConfig&) override;
    void symbolDependenciesChanged() override;
    void redoLayout() override;

    Bucket* getBucket(const style::Layer&) override;

    void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const optional<std::vector<std::string>>& layerIDs) override;

    void cancel() override;

    class LayoutResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
        std::unique_ptr<FeatureIndex> featureIndex;
        std::unique_ptr<GeometryTileData> tileData;
        uint64_t correlationID;
    };
    void onLayout(LayoutResult);

    class PlacementResult {
    public:
        std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;
        std::unique_ptr<CollisionTile> collisionTile;
        uint64_t correlationID;
    };
    void onPlacement(PlacementResult);

    void onError(std::exception_ptr);

private:
    const std::string sourceID;
    style::Style& style;

    // Used to signal the worker that it should abandon parsing this tile as soon as possible.
    std::atomic<bool> obsolete { false };

    std::shared_ptr<Mailbox> mailbox;
    Actor<GeometryTileWorker> worker;

    uint64_t correlationID = 0;
    optional<PlacementConfig> requestedConfig;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> nonSymbolBuckets;
    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTileData> data;

    std::unordered_map<std::string, std::shared_ptr<Bucket>> symbolBuckets;
    std::unique_ptr<CollisionTile> collisionTile;
};

} // namespace mbgl
