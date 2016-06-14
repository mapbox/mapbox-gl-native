#include <mbgl/style/sources/raster_source.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id_,
                           std::string url_,
                           uint16_t tileSize_,
                           std::unique_ptr<Tileset>&& tileset_)
    : Source(SourceType::Raster, std::move(id_), std::move(url_), tileSize_, std::move(tileset_)) {
}

} // namespace style
} // namespace mbgl
