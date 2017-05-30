#pragma once

#include <mbgl/style/image.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_light.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/map/zoom_history.hpp>
#include <mbgl/map/mode.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class FileSource;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;
class RenderData;
class TransformState;
class RenderedQueryOptions;
class Scheduler;
class UpdateParameters;
class RenderStyleObserver;

namespace style {
class Image;
class Source;
class Layer;
} // namespace style

class RenderStyle : public GlyphAtlasObserver,
                    public RenderSourceObserver {
public:
    RenderStyle(Scheduler&, FileSource&);
    ~RenderStyle() final;

    void setObserver(RenderStyleObserver*);
    void update(const UpdateParameters&);

    bool isLoaded() const;
    bool hasTransitions() const;

    RenderSource* getRenderSource(const std::string& id) const;

    std::vector<const RenderLayer*> getRenderLayers() const;

          RenderLayer* getRenderLayer(const std::string& id);
    const RenderLayer* getRenderLayer(const std::string& id) const;

    const RenderLight& getRenderLight() const;

    RenderData getRenderData(MapDebugOptions, float angle);

    std::vector<Feature> queryRenderedFeatures(const ScreenLineString& geometry,
                                               const TransformState& transformState,
                                               const RenderedQueryOptions& options) const;

    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    void dumpDebugLogs() const;

    Scheduler& scheduler;
    FileSource& fileSource;
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    std::unique_ptr<LineAtlas> lineAtlas;

private:
    std::vector<Immutable<style::Image::Impl>> imageImpls;
    std::vector<Immutable<style::Source::Impl>> sourceImpls;
    std::vector<Immutable<style::Layer::Impl>> layerImpls;

    std::unordered_map<std::string, std::unique_ptr<RenderSource>> renderSources;
    std::unordered_map<std::string, std::unique_ptr<RenderLayer>> renderLayers;
    RenderLight renderLight;

    // GlyphAtlasObserver implementation.
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // RenderSourceObserver implementation.
    void onTileChanged(RenderSource&, const OverscaledTileID&) override;
    void onTileError(RenderSource&, const OverscaledTileID&, std::exception_ptr) override;

    RenderStyleObserver* observer;
    ZoomHistory zoomHistory;
};

} // namespace mbgl
