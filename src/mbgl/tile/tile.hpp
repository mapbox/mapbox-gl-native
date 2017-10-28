#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_necessity.hpp>
#include <mbgl/renderer/tile_mask.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace mbgl {

class DebugBucket;
class TransformState;
class TileObserver;
class RenderLayer;
class RenderedQueryOptions;
class SourceQueryOptions;

class CollisionIndex;
struct CollisionFadeTimes;

namespace gl {
class Context;
} // namespace gl

class Tile : private util::noncopyable {
public:
    Tile(OverscaledTileID);
    virtual ~Tile();

    void setObserver(TileObserver* observer);

    virtual void setNecessity(TileNecessity) {}

    // Mark this tile as no longer needed and cancel any pending work.
    virtual void cancel() = 0;

    virtual void upload(gl::Context&) = 0;
    virtual Bucket* getBucket(const style::Layer::Impl&) const = 0;

    virtual void setShowCollisionBoxes(const bool) {}
    virtual void setLayers(const std::vector<Immutable<style::Layer::Impl>>&) {}
    virtual void setMask(TileMask&&) {}

    // TODO: Implement
    virtual void placeLayer(const bool, CollisionIndex& , const style::Layer::Impl&) {};
    virtual void commitPlacement(const CollisionIndex&, CollisionFadeTimes&) {};

    virtual void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const std::vector<const RenderLayer*>&,
            const RenderedQueryOptions& options,
            const CollisionIndex&);

    virtual void querySourceFeatures(
            std::vector<Feature>& result,
            const SourceQueryOptions&);

    void setTriedCache();

    // Returns true when the tile source has received a first response, regardless of whether a load
    // error occurred or actual data was loaded.
    bool hasTriedCache() const {
        return triedOptional;
    }

    // Tile data considered "Renderable" can be used for rendering. Data in
    // partial state is still waiting for network resources but can also
    // be rendered, although layers will be missing.
    bool isRenderable() const {
        return renderable;
    }

    // A tile is "Loaded" when we have received a response from a FileSource, and have attempted to
    // parse the tile (if applicable). Tile implementations should set this to true when a load
    // error occurred, or after the tile was parsed successfully.
    bool isLoaded() const {
        return loaded;
    }

    // "Completion" of a tile means that we have attempted to load it, and parsed it completely,
    // i.e. no parsing or placement operations are pending for that tile.
    // Completeness doesn't mean that the tile can be rendered, but merely that we have exhausted
    // all options to get this tile to a renderable state. Some tiles may not be renderable, but
    // complete, e.g. when a raster tile couldn't be loaded, or parsing failed.
    bool isComplete() const {
        return loaded && !pending;
    }

    void dumpDebugLogs() const;

    const OverscaledTileID id;
    optional<Timestamp> modified;
    optional<Timestamp> expires;

    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;

protected:
    bool triedOptional = false;
    bool renderable = false;
    bool pending = false;
    bool loaded = false;

    TileObserver* observer = nullptr;
};

} // namespace mbgl
