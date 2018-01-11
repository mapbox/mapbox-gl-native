#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/style/sources/raster_dem_source_impl.hpp>

namespace mbgl {

enum class DEMTileNeighbors {
  // 0b00000001
  Left = 1 << 0,
  // 0b00000010
  Right = 1 << 1,
  // 0b00000100
  TopLeft = 1 << 2,
  // 0b00001000
  TopCenter = 1 << 3,
  // 0b00010000
  TopRight = 1 << 4,
  // 0b00100000
  BottomLeft = 1 << 5,
  // 0b01000000
  BottomCenter = 1 << 6,
  // 0b10000000
  BottomRight = 1 << 7
};

inline unsigned char operator|(unsigned char a, DEMTileNeighbors b) {
    return static_cast<unsigned char>(a | int(b));
};

inline unsigned char operator&(unsigned char a, DEMTileNeighbors b) {
    return static_cast<unsigned char>(a & int(b));
}

inline bool operator!=(unsigned char a, DEMTileNeighbors b) {
    return a != static_cast<unsigned char>(b);
}


class RenderRasterDEMSource : public RenderSource {
public:
    RenderRasterDEMSource(Immutable<style::RasterDEMSource::Impl>);

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
                          const std::vector<const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const CollisionIndex& collisionIndex) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void onLowMemory() final;
    void dumpDebugLogs() const final;

private:
    const style::RasterDEMSource::Impl& impl() const;

    TilePyramid tilePyramid;
    optional<std::vector<std::string>> tileURLTemplates;

protected:
    void onTileChanged(Tile&) final;
};

template <>
inline bool RenderSource::is<RenderRasterDEMSource>() const {
    return baseImpl->type == style::SourceType::RasterDEM;
}

} // namespace mbgl
