#pragma once

#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/renderer/sources/render_tile_source.hpp>

namespace mbgl {

class RenderAnnotationSource final : public RenderTileSource {
public:
    explicit RenderAnnotationSource(Immutable<AnnotationSource::Impl>);

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<std::string, const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

private:
    const AnnotationSource::Impl& impl() const;
};

} // namespace mbgl
