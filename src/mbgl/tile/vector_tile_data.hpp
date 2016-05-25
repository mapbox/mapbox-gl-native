#pragma once

#include <mbgl/tile/tile_data.hpp>
#include <mbgl/tile/tile_worker.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/util/feature.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>

namespace mbgl {

class Style;
class AsyncRequest;
class GeometryTileMonitor;
class FeatureIndex;

class VectorTileData : public TileData {
public:
    VectorTileData(const OverscaledTileID&,
                   std::unique_ptr<GeometryTileMonitor> monitor,
                   std::string sourceID,
                   Style&,
                   const MapMode,
                   const std::function<void(std::exception_ptr)>& callback);

    ~VectorTileData();

    Bucket* getBucket(const StyleLayer&) override;

    bool parsePending(std::function<void(std::exception_ptr)> callback) override;

    void redoPlacement(PlacementConfig config, const std::function<void()>&) override;
    void redoPlacement(const std::function<void()>&) override;

    bool hasData() const override;

    void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const optional<std::vector<std::string>>& layerIDs) override;

    void cancel() override;

private:
    Style& style;
    Worker& worker;
    TileWorker tileWorker;

    std::unique_ptr<GeometryTileMonitor> monitor;
    std::unique_ptr<AsyncRequest> tileRequest;
    std::unique_ptr<AsyncRequest> workRequest;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTile> geometryTile;

    // Stores the placement configuration of the text that is currently placed on the screen.
    PlacementConfig placedConfig;

    // Stores the placement configuration of how the text should be placed. This isn't necessarily
    // the one that is being displayed.
    PlacementConfig targetConfig;
};

} // namespace mbgl
