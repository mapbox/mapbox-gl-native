#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/tile/tile_id.hpp>
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
class PlacementConfig;
class RenderStyle;
class RenderedQueryOptions;
class SourceQueryOptions;

namespace gl {
class Context;
} // namespace gl

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

    virtual void upload(gl::Context&) = 0;
    virtual Bucket* getBucket(const style::Layer::Impl&) const = 0;

    virtual void setPlacementConfig(const PlacementConfig&) {}
    virtual void setLayers(const std::vector<Immutable<style::Layer::Impl>>&) {}
    virtual void setMask(TileMask&&) {}

    virtual void queryRenderedFeatures(
            std::unordered_map<std::string, std::vector<Feature>>& result,
            const GeometryCoordinates& queryGeometry,
            const TransformState&,
            const RenderStyle&,
            const RenderedQueryOptions& options);

    virtual void querySourceFeatures(
            std::vector<Feature>& result,
            const SourceQueryOptions&);

    // Tile data considered "Renderable" can be used for rendering. It doesn't necessarily mean that
    // there is anything to render, since tiles can be empty. Tiles that are still waiting for
    // network resources but can also be rendered, although layers will be missing.
    bool isRenderable() const {
        return renderable;
    }

    // A tile is "loaded" when we have received a response from a FileSource, and have attempted
    // to parse the tile (if applicable), and there are no further operations in progress to acquire
    // more, newer, or better data. Furthermore, it means that there are no pending layout/placement
    // or parsing operations going on. Completeness doesn't mean that the tile can be rendered, but
    // merely that we have exhausted all options to get this tile to a renderable state. Some tiles
    // may not be renderable, but complete, e.g. when a raster tile couldn't be loaded, or parsing
    // failed.
    bool isLoaded() const {
        return (loaded ? parsed : failed) && !pending;
    }

    void dumpDebugLogs() const;

    void logDebug(const char*) const;

    // This information is only used for displaying debugging information.
    const OverscaledTileID id;
    optional<Timestamp> modified;
    optional<Timestamp> expires;

    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;
    
    virtual float yStretch() const { return 1.0f; }

protected:
    // A tile is loaded when we acquired data for it, and no further operations to acquire more,
    // newer, or better data are in progress.
    bool loaded = false;

    // A tile is marked as failed when we tried to acquire data, but couldn't, and no further
    // operations to acquire more, newer, or better data are in progress.
    bool failed = false;

    // A tile is marked as parsed when we attempted to parse its contents. It doesn't necessairly
    // mean that we *successfully* parsed it, just that there's been an attempt.
    bool parsed = false;

    // Renderability means that we successfully parsed a tile. Note that it doesn't mean there's
    // anything to render (e.g. vector tiles may be empty).
    bool renderable = false;

    // A tile is pending while parsing or placement operations are going on.
    bool pending = false;

    TileObserver* observer = nullptr;
};

} // namespace mbgl
