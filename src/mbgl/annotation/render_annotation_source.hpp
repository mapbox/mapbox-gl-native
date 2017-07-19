#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/annotation/annotation_source.hpp>

namespace mbgl {

class RenderAnnotationSource : public RenderSource {
public:
    RenderAnnotationSource(Immutable<AnnotationSource::Impl>);

    bool isLoaded() const final;

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::Layer::Impl>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    void startRender(PaintParameters&) final;
    void finishRender(PaintParameters&) final;

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderStyle& style,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void onLowMemory() final;
    void dumpDebugLogs() const final;

private:
    const AnnotationSource::Impl& impl() const;

    TilePyramid tilePyramid;
};

template <>
inline bool RenderSource::is<RenderAnnotationSource>() const {
    return baseImpl->type == SourceType::Annotations;
}

} // namespace mbgl
