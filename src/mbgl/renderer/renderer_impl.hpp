#pragma once

#include <mbgl/renderer/mode.hpp>
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
#include <mbgl/text/placement.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class RendererBackend;
class RendererObserver;
class RenderSource;
class RenderLayer;
class UpdateParameters;
class RenderStaticData;
class RenderedQueryOptions;
class SourceQueryOptions;
class FileSource;
class Scheduler;
class GlyphManager;
class ImageManager;
class LineAtlas;
class CrossTileSymbolIndex;

class Renderer::Impl : public GlyphManagerObserver,
                       public RenderSourceObserver{
public:
    Impl(RendererBackend&, float pixelRatio_, FileSource&, Scheduler&, GLContextMode,
         const optional<std::string> programCacheDir, const optional<std::string> localFontFamily);
    ~Impl() final;

    void markContextLost() {
        contextLost = true;
    };

    void setObserver(RendererObserver*);

    void render(const UpdateParameters&);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    std::vector<Feature> queryShapeAnnotations(const ScreenLineString&) const;

    void reduceMemoryUse();
    void dumDebugLogs();

private:
    bool isLoaded() const;
    bool hasTransitions(TimePoint) const;

    RenderSource* getRenderSource(const std::string& id) const;

          RenderLayer* getRenderLayer(const std::string& id);
    const RenderLayer* getRenderLayer(const std::string& id) const;
              
    void queryRenderedSymbols(std::unordered_map<std::string, std::vector<Feature>>& resultsByLayer,
                              const ScreenLineString& geometry,
                              const std::vector<const RenderLayer*>& layers,
                              const RenderedQueryOptions& options) const;
    
    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&, const std::vector<const RenderLayer*>&) const;

    // GlyphManagerObserver implementation.
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // RenderSourceObserver implementation.
    void onTileChanged(RenderSource&, const OverscaledTileID&) override;
    void onTileError(RenderSource&, const OverscaledTileID&, std::exception_ptr) override;

    void updateFadingTiles();

    friend class Renderer;

    RendererBackend& backend;
    Scheduler& scheduler;
    FileSource& fileSource;

    RendererObserver* observer;

    const GLContextMode contextMode;
    const float pixelRatio;
    const optional<std::string> programCacheDir;

    enum class RenderState {
        Never,
        Partial,
        Fully,
    };

    RenderState renderState = RenderState::Never;
    ZoomHistory zoomHistory;
    TransformState transformState;

    std::unique_ptr<GlyphManager> glyphManager;
    std::unique_ptr<ImageManager> imageManager;
    std::unique_ptr<LineAtlas> lineAtlas;
    std::unique_ptr<RenderStaticData> staticData;

    Immutable<std::vector<Immutable<style::Image::Impl>>> imageImpls;
    Immutable<std::vector<Immutable<style::Source::Impl>>> sourceImpls;
    Immutable<std::vector<Immutable<style::Layer::Impl>>> layerImpls;

    std::unordered_map<std::string, std::unique_ptr<RenderSource>> renderSources;
    std::unordered_map<std::string, std::unique_ptr<RenderLayer>> renderLayers;
    RenderLight renderLight;

    CrossTileSymbolIndex crossTileSymbolIndex;
    std::unique_ptr<Placement> placement;

    bool contextLost = false;
    bool fadingTiles = false;
};

} // namespace mbgl
