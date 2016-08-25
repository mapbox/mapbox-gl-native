#pragma once

#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_worker.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/util/feature.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

namespace mbgl {

class AsyncRequest;
class GeometryTileData;
class FeatureIndex;

namespace style {
class Style;
class Layer;
} // namespace style

class GeometryTile : public Tile {
public:
    GeometryTile(const OverscaledTileID&,
                 std::string sourceID,
                 style::Style&,
                 const MapMode);

    ~GeometryTile() override;

    void setError(std::exception_ptr err);
    void setData(std::unique_ptr<const GeometryTileData>);

    Bucket* getBucket(const style::Layer&) override;

    bool parsePending() override;

    void redoPlacement(PlacementConfig) override;

    void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const optional<std::vector<std::string>>& layerIDs) override;

    void cancel() override;

private:
    std::vector<std::unique_ptr<style::Layer>> cloneStyleLayers() const;
    void redoPlacement();

    const std::string sourceID;
    style::Style& style;
    Worker& worker;
    TileWorker tileWorker;

    std::unique_ptr<AsyncRequest> workRequest;

    // Contains all the Bucket objects for the tile. Buckets are render
    // objects and they get added by tile parsing operations.
    std::unordered_map<std::string, std::unique_ptr<Bucket>> buckets;

    std::unique_ptr<FeatureIndex> featureIndex;
    std::unique_ptr<const GeometryTileData> data;

    // Stores the placement configuration of the text that is currently placed on the screen.
    PlacementConfig placedConfig;

    // Stores the placement configuration of how the text should be placed. This isn't necessarily
    // the one that is being displayed.
    PlacementConfig targetConfig;

    // Used to signal the worker that it should abandon parsing this tile as soon as possible.
    std::atomic<bool> obsolete { false };
};

} // namespace mbgl
