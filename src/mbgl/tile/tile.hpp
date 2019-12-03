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
#include <mbgl/style/layer_properties.hpp>

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace mbgl {

class LayerRenderData;
class TransformState;
class TileObserver;
class RenderLayer;
class TileRenderData;
class RenderedQueryOptions;
class SourceQueryOptions;
class CollisionIndex;
class SourceFeatureState;

namespace gfx {
class UploadPass;
} // namespace gfx

class Tile {
public:
    enum class Kind : uint8_t {
        Geometry,
        Raster,
        RasterDEM
    };
    Tile(const Tile&) = delete;
    Tile& operator=(const Tile&) = delete;

    Tile(Kind, OverscaledTileID);
    virtual ~Tile();

    virtual std::unique_ptr<TileRenderData> createRenderData() = 0;

    void setObserver(TileObserver* observer);

    virtual void setNecessity(TileNecessity) {}

    // Mark this tile as no longer needed and cancel any pending work.
    virtual void cancel();

    // Notifies this tile of the updated layer properties.
    //
    // Tile implementation should update the contained layer
    // render data with the given properties.
    // 
    // Returns `true` if the corresponding render layer data is present in this tile (and i.e. it
    // was succesfully updated); returns `false` otherwise.
    virtual bool layerPropertiesUpdated(const Immutable<style::LayerProperties>& layerProperties) = 0;
    virtual void setShowCollisionBoxes(const bool) {}
    virtual void setLayers(const std::vector<Immutable<style::LayerProperties>>&) {}
    virtual void setMask(TileMask&&) {}

    virtual void queryRenderedFeatures(std::unordered_map<std::string, std::vector<Feature>>& result,
                                       const GeometryCoordinates& queryGeometry, const TransformState&,
                                       const std::unordered_map<std::string, const RenderLayer*>&,
                                       const RenderedQueryOptions& options, const mat4& projMatrix,
                                       const SourceFeatureState& featureState);

    virtual void querySourceFeatures(
            std::vector<Feature>& result,
            const SourceQueryOptions&);

    virtual float getQueryPadding(const std::unordered_map<std::string, const RenderLayer*>&);

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
    
    // "holdForFade" is used to keep tiles in the render tree after they're no longer
    // ideal tiles in order to allow symbols to fade out
    virtual bool holdForFade() const {
        return false;
    }
    // Set whenever this tile is used as an ideal tile
    virtual void markRenderedIdeal() {}
    // Set when the tile is removed from the ideal render set but may still be held for fading
    virtual void markRenderedPreviously() {}
    // Placement operation performed while this tile is fading
    // We hold onto a tile for two placements: fading starts with the first placement
    // and will have time to finish by the second placement.
    virtual void performedFadePlacement() {}

    virtual void setFeatureState(const LayerFeatureStates&) {}

    void dumpDebugLogs() const;

    const Kind kind;
    OverscaledTileID id;
    optional<Timestamp> modified;
    optional<Timestamp> expires;
    // Indicates whether this tile is used for the currently visible layers on the map.
    // Re-initialized at every source update.
    bool usedByRenderedLayers = false;

protected:
    bool triedOptional = false;
    bool renderable = false;
    bool pending = false;
    bool loaded = false;


    TileObserver* observer = nullptr;
};

} // namespace mbgl
