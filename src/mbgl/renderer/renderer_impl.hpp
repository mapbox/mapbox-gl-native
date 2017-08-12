#pragma once

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/zoom_history.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/text/glyph_manager_observer.hpp>
#include <mbgl/style/image.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class UpdateParameters;
class PaintParameters;
class RenderStaticData;
class RenderData;
class RenderedQueryOptions;
class SourceQueryOptions;

class FileSource;
class GlyphManager;
class ImageManager;
class LineAtlas;
class Scheduler;

namespace style {
class Image;
class Source;
class Layer;
} // namespace style

class Renderer::Impl : public GlyphManagerObserver,
                       public RenderSourceObserver{
public:
    Impl(RendererBackend&, float pixelRatio_, FileSource&, Scheduler&, GLContextMode,
         const optional<std::string> programCacheDir);
    ~Impl() final;

    void setObserver(RendererObserver*);

    void render(const UpdateParameters&);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;

    void onLowMemory();
    void dumDebugLogs();

private:
    void doRender(PaintParameters&);

    bool isLoaded() const;
    bool hasTransitions() const;

    RenderSource* getRenderSource(const std::string& id) const;

          RenderLayer* getRenderLayer(const std::string& id);
    const RenderLayer* getRenderLayer(const std::string& id) const;

    // GlyphManagerObserver implementation.
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // RenderSourceObserver implementation.
    void onTileChanged(RenderSource&, const OverscaledTileID&) override;
    void onTileError(RenderSource&, const OverscaledTileID&, std::exception_ptr) override;

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
    FrameHistory frameHistory;
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
};

} // namespace mbgl
