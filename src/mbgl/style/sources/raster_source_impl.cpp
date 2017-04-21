#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/renderer/sources/render_raster_source.hpp>

namespace mbgl {
namespace style {

RasterSource::Impl::Impl(std::string id_, Source& base_,
                         variant<std::string, Tileset> urlOrTileset_,
                         uint16_t tileSize_)
    : TileSourceImpl(SourceType::Raster, std::move(id_), base_, std::move(urlOrTileset_), tileSize_) {
}

std::unique_ptr<RenderSource> RasterSource::Impl::createRenderSource() const {
    return std::make_unique<RenderRasterSource>(*this);
}

} // namespace style
} // namespace mbgl
