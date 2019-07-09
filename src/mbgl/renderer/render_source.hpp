#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/layer_properties.hpp>

#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

namespace mbgl {

class PaintParameters;
class TransformState;
class RenderTile;
class RenderLayer;
class RenderedQueryOptions;
class SourceQueryOptions;
class Tile;
class RenderSourceObserver;
class TileParameters;
class CollisionIndex;
class TransformParameters;
class ImageManager;
class ImageSourceRenderData;
class RenderItem;

namespace gfx {
class UploadPass;
} // namespace gfx

class SourcePrepareParameters {
public:
    const TransformParameters& transform;
    const MapDebugOptions& debugOptions;
    const ImageManager& imageManager;
};

using RenderTiles = std::shared_ptr<const std::vector<std::reference_wrapper<const RenderTile>>>;

class RenderSource : protected TileObserver {
public:
    static std::unique_ptr<RenderSource> create(Immutable<style::Source::Impl>);
    virtual ~RenderSource();

    bool isEnabled() const;
    virtual bool isLoaded() const = 0;

    virtual void update(Immutable<style::Source::Impl>,
                        const std::vector<Immutable<style::LayerProperties>>&,
                        bool needsRendering,
                        bool needsRelayout,
                        const TileParameters&) = 0;
    // Note: calling of this method might reset the contained render data.
    virtual std::unique_ptr<RenderItem> createRenderItem() = 0;
    // Creates the render data to be passed to the render item.
    virtual void prepare(const SourcePrepareParameters&) = 0;
    virtual void updateFadingTiles() = 0;
    virtual bool hasFadingTiles() const = 0;
    // If supported, returns a shared list of RenderTiles, sorted by tile id and excluding tiles hold for fade;
    // returns nullptr otherwise.
    virtual RenderTiles getRenderTiles() const { return nullptr; }
    // If supported, returns a shared list of RenderTiles, sorted in opposite y position, so tiles with overlapping
    // symbols are drawn on top of each other, with lower symbols being drawn on top of higher symbols;
    // returns nullptr otherwise.
    virtual RenderTiles getRenderTilesSortedByYPosition() const { return nullptr; }
    // If supported, returns pointer to image data; returns nullptr otherwise.
    virtual const ImageSourceRenderData* getImageRenderData() const { return nullptr; }
    virtual const Tile* getRenderedTile(const UnwrappedTileID&) const { return nullptr; }

    virtual std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<std::string, const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const = 0;

    virtual std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const = 0;

    virtual FeatureExtensionValue
    queryFeatureExtensions(const Feature&,
                           const std::string&,
                           const std::string&,
                           const optional<std::map<std::string, Value>>&) const {
        return {};
    }

    virtual void reduceMemoryUse() = 0;

    virtual void dumpDebugLogs() const = 0;

    virtual uint8_t getMaxZoom() const;

    void setObserver(RenderSourceObserver*);

    Immutable<style::Source::Impl> baseImpl;

protected:
    RenderSource(Immutable<style::Source::Impl>);
    RenderSourceObserver* observer;

    bool enabled = false;

    void onTileChanged(Tile&) override;
    void onTileError(Tile&, std::exception_ptr) final;
};

} // namespace mbgl
