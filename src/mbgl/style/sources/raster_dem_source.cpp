#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/mapbox.hpp>

namespace mbgl {
namespace style {

RasterDEMSource::RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize)
    : RasterSource(std::move(id), urlOrTileset_, tileSize, SourceType::RasterDEM){
}



} // namespace style
} // namespace mbgl
