#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace mbgl {

class Worker;
class DebugBucket;
class TransformState;
class TileDataObserver;

namespace style {
class Layer;
}

class TileData : private util::noncopyable {
public:
    TileData(const OverscaledTileID&);
    virtual ~TileData();

    void setObserver(TileDataObserver* observer);

    enum class Necessity : bool {
        Optional = false,
        Required = true,
    };

    virtual void setNecessity(Necessity) = 0;

    // Mark this tile as no longer needed and cancel any pending work.
    virtual void cancel() = 0;

    virtual Bucket* getBucket(const style::Layer&) = 0;

    virtual bool parsePending() { return true; }
    virtual void redoPlacement(PlacementConfig) {}
    virtual void redoPlacement() {}

    virtual void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const optional<std::vector<std::string>>& layerIDs);

    void setTriedOptional();

    // Returns true when the tile source has received a first response, regardless of whether a load
    // error occurred or actual data was loaded.
    bool hasTriedOptional() const {
        return triedOptional;
    }

    // Tile data considered "Renderable" can be used for rendering. Data in
    // partial state is still waiting for network resources but can also
    // be rendered, although layers will be missing.
    bool isRenderable() const {
        return availableData != DataAvailability::None;
    }

    bool isComplete() const {
        return availableData == DataAvailability::All;
    }
    bool isIncomplete() const {
        return availableData == DataAvailability::Some;
    }

    void dumpDebugLogs() const;

    const OverscaledTileID id;
    optional<Timestamp> modified;
    optional<Timestamp> expires;

    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;

protected:
    bool triedOptional = false;

    enum class DataAvailability : uint8_t {
        // Still waiting for data to load or parse.
        None,

        // TileData is partially parsed, some buckets are still waiting for dependencies
        // to arrive, but it is good for rendering. Partial tiles can also be re-parsed,
        // but might remain in the same state if dependencies are still missing.
        Some,

        // TileData is fully parsed, and all buckets are available if they exist.
        All,
    };

    DataAvailability availableData = DataAvailability::None;

    TileDataObserver* observer = nullptr;
};

} // namespace mbgl
