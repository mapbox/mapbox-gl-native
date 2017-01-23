#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize)
    : Source(SourceType::Raster, std::make_unique<RasterSource::Impl>(std::move(id), *this, std::move(urlOrTileset), tileSize)),
      impl(static_cast<Impl*>(baseImpl.get())) {
}

optional<std::string> RasterSource::getURL() const {
    auto urlOrTileset = impl->getURLOrTileset();
    if (urlOrTileset.is<std::string>()) {
        return urlOrTileset.get<std::string>();
    } else {
        return {};
    }
}

} // namespace style
} // namespace mbgl
