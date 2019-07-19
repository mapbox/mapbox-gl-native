#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {
namespace style {

RasterSource::Impl::Impl(SourceType sourceType, std::string id_, uint16_t tileSize_)
    : Source::Impl(sourceType, std::move(id_)),
      tileSize(tileSize_) {
}

RasterSource::Impl::Impl(const Impl& other, Tileset tileset_)
    : Source::Impl(other),
      tileset(std::move(tileset_)),
      tileSize(other.tileSize) {
}

uint16_t RasterSource::Impl::getTileSize() const {
    return tileSize;
}

optional<std::string> RasterSource::Impl::getAttribution() const {
    if (!tileset) {
        return {};
    }
    return tileset->attribution;
}

} // namespace style
} // namespace mbgl
