#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {

/**
 * @brief Base class for render sources that provide render tiles.
 */
class RenderTileSource : public RenderSource {
public:
    RenderTileSource(Immutable<style::Source::Impl>);
    ~RenderTileSource() override;

    bool isLoaded() const override;

    void upload(gfx::UploadPass&) override;
    void prepare(const SourcePrepareParameters&) override;
    void finishRender(PaintParameters&) override;
    void updateFadingTiles() override;
    bool hasFadingTiles() const override;

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() override;
    const Tile* getRenderedTile(const UnwrappedTileID&) const override;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const override;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const override;

    void reduceMemoryUse() override;
    void dumpDebugLogs() const override;

protected:
    TilePyramid tilePyramid;
    std::vector<RenderTile> renderTiles;
};

} // namespace mbgl
