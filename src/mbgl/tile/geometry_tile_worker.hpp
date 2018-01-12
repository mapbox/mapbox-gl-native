#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/style/layer_impl.hpp>

#include <atomic>
#include <memory>

namespace mbgl {

class GeometryTile;
class GeometryTileData;
class SymbolLayout;

namespace style {
class Layer;
} // namespace style

class GeometryTileWorker {
public:
    GeometryTileWorker(ActorRef<GeometryTileWorker> self,
                       ActorRef<GeometryTile> parent,
                       OverscaledTileID,
                       const std::string&,
                       const std::atomic<bool>&,
                       const MapMode,
                       const float pixelRatio,
                       const bool showCollisionBoxes_);
    ~GeometryTileWorker();

    void setLayers(std::vector<Immutable<style::Layer::Impl>>, uint64_t correlationID);
    void setData(std::unique_ptr<const GeometryTileData>, uint64_t correlationID);
    void setShowCollisionBoxes(bool showCollisionBoxes_, uint64_t correlationID_);
    
    void onGlyphsAvailable(GlyphMap glyphs);
    void onImagesAvailable(ImageMap images, uint64_t imageCorrelationID);

private:
    void coalesced();
    void redoLayout();
    void attemptPlacement();
    
    void coalesce();

    void requestNewGlyphs(const GlyphDependencies&);
    void requestNewImages(const ImageDependencies&);
   
    void symbolDependenciesChanged();
    bool hasPendingSymbolDependencies() const;

    ActorRef<GeometryTileWorker> self;
    ActorRef<GeometryTile> parent;

    const OverscaledTileID id;
    const std::string sourceID;
    const std::atomic<bool>& obsolete;
    const MapMode mode;
    const float pixelRatio;

    enum State {
        Idle,
        Coalescing,
        NeedLayout,
        NeedPlacement
    };

    State state = Idle;
    uint64_t correlationID = 0;
    uint64_t imageCorrelationID = 0;

    // Outer optional indicates whether we've received it or not.
    optional<std::vector<Immutable<style::Layer::Impl>>> layers;
    optional<std::unique_ptr<const GeometryTileData>> data;

    bool symbolLayoutsNeedPreparation = false;
    std::vector<std::unique_ptr<SymbolLayout>> symbolLayouts;
    GlyphDependencies pendingGlyphDependencies;
    ImageDependencies pendingImageDependencies;
    GlyphMap glyphMap;
    ImageMap imageMap;
    
    bool showCollisionBoxes;
    bool firstLoad = true;
};

} // namespace mbgl
