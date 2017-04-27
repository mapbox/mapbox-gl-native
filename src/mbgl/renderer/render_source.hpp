#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/source_impl.hpp>

#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

namespace mbgl {

class Painter;
class TransformState;
class RenderTile;
class RenderedQueryOptions;
class SourceQueryOptions;
class Tile;
class RenderSourceObserver;
class TileParameters;

namespace algorithm {
class ClipIDGenerator;
} // namespace algorithm

class RenderSource : protected TileObserver {
public:
    RenderSource(const style::Source::Impl&);
    virtual ~RenderSource() = default;

    virtual bool isLoaded() const = 0;

    // Called when the camera has changed. May load new tiles, unload obsolete tiles, or
    // trigger re-placement of existing complete tiles.
    virtual void updateTiles(const TileParameters&) = 0;

    // Removes all tiles (by putting them into the cache).
    virtual void removeTiles() = 0;

    // Remove all tiles and clear the cache.
    virtual void invalidateTiles() = 0;

    // Request that all loaded tiles re-run the layout operation on the existing source
    // data with fresh style information.
    virtual void reloadTiles() = 0;

    virtual void startRender(algorithm::ClipIDGenerator&,
                             const mat4& projMatrix,
                             const mat4& clipMatrix,
                             const TransformState&) = 0;
    virtual void finishRender(Painter&) = 0;

    virtual std::map<UnwrappedTileID, RenderTile>& getRenderTiles() = 0;

    virtual std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderedQueryOptions& options) const = 0;

    virtual std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const = 0;

    virtual void setCacheSize(size_t) = 0;
    virtual void onLowMemory() = 0;

    virtual void dumpDebugLogs() const = 0;

    void setObserver(RenderSourceObserver*);

    const style::Source::Impl& baseImpl;
    bool enabled = false;

protected:
    RenderSourceObserver* observer;

    void onTileChanged(Tile&) final;
    void onTileError(Tile&, std::exception_ptr) final;
};

} // namespace mbgl
