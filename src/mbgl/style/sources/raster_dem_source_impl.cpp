#include <mbgl/style/sources/raster_dem_source_impl.hpp>

namespace mbgl {
namespace style {

RasterDEMSource::Impl::Impl(std::string id_, uint16_t tileSize_)
    : Source::Impl(SourceType::RasterDEM, std::move(id_)),
      tileSize(tileSize_) {
}

RasterDEMSource::Impl::Impl(const Impl& other, Tileset tileset_)
    : Source::Impl(other),
      tileSize(other.tileSize),
      tileset(std::move(tileset_)) {
}

uint16_t RasterDEMSource::Impl::getTileSize() const {
    return tileSize;
}

optional<Tileset> RasterDEMSource::Impl::getTileset() const {
    return tileset;
}

optional<std::string> RasterDEMSource::Impl::getAttribution() const {
    if (!tileset) {
        return {};
    }
    return tileset->attribution;
}

} // namespace style
} // namespace mbgl
