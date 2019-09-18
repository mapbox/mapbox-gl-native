#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/mapbox.hpp>

namespace mbgl {
namespace style {

RasterDEMSource::RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize)
    : RasterSource(std::move(id), urlOrTileset_, tileSize, SourceType::RasterDEM){
}

bool RasterDEMSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(Tile::Kind::RasterDEM) == mbgl::underlying_type(info->tileKind);
}

} // namespace style
} // namespace mbgl
