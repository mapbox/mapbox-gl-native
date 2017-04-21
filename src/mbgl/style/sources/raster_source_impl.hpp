#pragma once

#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/tile_source_impl.hpp>

namespace mbgl {
namespace style {

class RasterSource::Impl : public TileSourceImpl {
public:
    Impl(std::string id, Source&, variant<std::string, Tileset>, uint16_t tileSize);

    std::unique_ptr<RenderSource> createRenderSource() const final;
};

} // namespace style
} // namespace mbgl
