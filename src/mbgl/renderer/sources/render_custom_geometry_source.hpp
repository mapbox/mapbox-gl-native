#pragma once

#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/style/sources/custom_geometry_source_impl.hpp>

namespace mbgl {

class RenderCustomGeometrySource final : public RenderTileSource {
public:
    explicit RenderCustomGeometrySource(Immutable<style::CustomGeometrySource::Impl>);

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) override;
    
private:
    const style::CustomGeometrySource::Impl& impl() const;
};

} // namespace mbgl
