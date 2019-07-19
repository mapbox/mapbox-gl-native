#pragma once

#include <mbgl/renderer/sources/render_tile_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {

class RenderVectorSource final : public RenderTileSource {
public:
    explicit RenderVectorSource(Immutable<style::VectorSource::Impl>);

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;
private:
    const style::VectorSource::Impl& impl() const;
    optional<Tileset> tileset;
};

} // namespace mbgl
