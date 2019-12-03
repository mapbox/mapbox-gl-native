#pragma once

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/zoom_history.hpp>
#include <mbgl/text/cross_tile_symbol_index.hpp>
#include <mbgl/text/glyph_manager_observer.hpp>
#include <mbgl/renderer/image_manager_observer.hpp>
#include <mbgl/text/placement.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class RendererObserver;
class RenderSource;
class RenderLayer;
class UpdateParameters;
class RenderStaticData;
class RenderedQueryOptions;
class SourceQueryOptions;
class GlyphManager;
class ImageManager;
class LineAtlas;
class PatternAtlas;
class CrossTileSymbolIndex;
class RenderTree;

namespace style {
    class LayerProperties;
} // namespace style

class RenderOrchestrator final : public GlyphManagerObserver,
                                 public ImageManagerObserver,
                                 public RenderSourceObserver {
public:
    RenderOrchestrator(
        bool backgroundLayerAsColor_,
        optional<std::string> localFontFamily_);
    ~RenderOrchestrator() override;

    void markContextLost() {
        contextLost = true;
    };
    // TODO: Introduce RenderOrchestratorObserver.
    void setObserver(RendererObserver*);

    std::unique_ptr<RenderTree> createRenderTree(const UpdateParameters&);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    std::vector<Feature> queryShapeAnnotations(const ScreenLineString&) const;

    FeatureExtensionValue queryFeatureExtensions(const std::string& sourceID,
                                                 const Feature& feature,
                                                 const std::string& extension,
                                                 const std::string& extensionField,
                                                 const optional<std::map<std::string, Value>>& args) const;

    void setFeatureState(const std::string& sourceID, const optional<std::string>& layerID,
                         const std::string& featureID, const FeatureState& state);

    void getFeatureState(FeatureState& state, const std::string& sourceID, const optional<std::string>& layerID,
                         const std::string& featureID) const;

    void removeFeatureState(const std::string& sourceID, const optional<std::string>& sourceLayerID,
                            const optional<std::string>& featureID, const optional<std::string>& stateKey);

    void reduceMemoryUse();
    void dumpDebugLogs();

private:
    bool isLoaded() const;
    bool hasTransitions(TimePoint) const;

    RenderSource* getRenderSource(const std::string& id) const;

          RenderLayer* getRenderLayer(const std::string& id);
    const RenderLayer* getRenderLayer(const std::string& id) const;
              
    void queryRenderedSymbols(std::unordered_map<std::string, std::vector<Feature>>& resultsByLayer,
                              const ScreenLineString& geometry,
                              const std::unordered_map<std::string, const RenderLayer*>& layers,
                              const RenderedQueryOptions& options) const;
    
    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&, const std::unordered_map<std::string, const RenderLayer*>&) const;

    // GlyphManagerObserver implementation.
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // RenderSourceObserver implementation.
    void onTileChanged(RenderSource&, const OverscaledTileID&) override;
    void onTileError(RenderSource&, const OverscaledTileID&, std::exception_ptr) override;

    // ImageManagerObserver implementation
    void onStyleImageMissing(const std::string&, std::function<void()>) override;
    void onRemoveUnusedStyleImages(const std::vector<std::string>&) override;

    RendererObserver* observer;

    ZoomHistory zoomHistory;
    TransformState transformState;

    std::unique_ptr<GlyphManager> glyphManager;
    std::unique_ptr<ImageManager> imageManager;
    std::unique_ptr<LineAtlas> lineAtlas;
    std::unique_ptr<PatternAtlas> patternAtlas;

    Immutable<std::vector<Immutable<style::Image::Impl>>> imageImpls;
    Immutable<std::vector<Immutable<style::Source::Impl>>> sourceImpls;
    Immutable<std::vector<Immutable<style::Layer::Impl>>> layerImpls;

    std::unordered_map<std::string, std::unique_ptr<RenderSource>> renderSources;
    std::unordered_map<std::string, std::unique_ptr<RenderLayer>> renderLayers;
    RenderLight renderLight;

    CrossTileSymbolIndex crossTileSymbolIndex;
    PlacementController placementController;

    const bool backgroundLayerAsColor;
    bool contextLost = false;

    // Vectors with reserved capacity of layerImpls->size() to avoid reallocation
    // on each frame.
    std::vector<Immutable<style::LayerProperties>> filteredLayersForSource;
    std::vector<std::reference_wrapper<RenderLayer>> orderedLayers;
    std::vector<std::reference_wrapper<RenderLayer>> layersNeedPlacement;
};

} // namespace mbgl
