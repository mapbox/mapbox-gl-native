#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/storage/resource.hpp>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace mbgl {

class DebugBucket;
class TransformState;
class TileObserver;
class PlacementConfig;

namespace style {
class Layer;
} // namespace style

class Tile : private util::noncopyable {
public:
    Tile(OverscaledTileID);
    virtual ~Tile();

    void setObserver(TileObserver* observer);

    // Tiles can have two states: optional or required.
    // - optional means that only low-cost actions should be taken to obtain the data
    //   (e.g. load from cache, but accept stale data)
    // - required means that every effort should be taken to obtain the data (e.g. load
    //   from internet and keep the data fresh if it expires)
    using Necessity = Resource::Necessity;

    virtual void setNecessity(Necessity) = 0;

    // Mark this tile as no longer needed and cancel any pending work.
    virtual void cancel() = 0;

    virtual Bucket* getBucket(const style::Layer&) = 0;

    virtual void setPlacementConfig(const PlacementConfig&) {}
    virtual void symbolDependenciesChanged() {};
    virtual void redoLayout() {}

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

        // Tile is partially parsed, some buckets are still waiting for dependencies
        // to arrive, but it is good for rendering. Partial tiles can also be re-parsed,
        // but might remain in the same state if dependencies are still missing.
        Some,

        // Tile is fully parsed, and all buckets are available if they exist.
        All,
    };

    DataAvailability availableData = DataAvailability::None;

    TileObserver* observer = nullptr;
};

} // namespace mbgl
