#include <cstring>
#include <map>
#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/style/custom_tile_loader.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/sources/custom_geometry_source_impl.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <tuple>

namespace mbgl {
namespace style {

CustomGeometrySource::CustomGeometrySource(std::string id,
                                       const CustomGeometrySource::Options options)
    : Source(makeMutable<CustomGeometrySource::Impl>(std::move(id), options)),
    loader(std::make_unique<Actor<CustomTileLoader>>(Scheduler::GetBackground(), options.fetchTileFunction, options.cancelTileFunction)) {
}

CustomGeometrySource::~CustomGeometrySource() = default;

const CustomGeometrySource::Impl& CustomGeometrySource::impl() const {
    return static_cast<const CustomGeometrySource::Impl&>(*baseImpl);
}

void CustomGeometrySource::loadDescription(FileSource&) {
    baseImpl = makeMutable<CustomGeometrySource::Impl>(impl(), loader->self());
    loaded = true;
    observer->onSourceLoaded(*this);
}

bool CustomGeometrySource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return !std::strcmp(info->type, "line") || !std::strcmp(info->type, "symbol") ||
           !std::strcmp(info->type, "circle") || !std::strcmp(info->type, "fill") ||
           !std::strcmp(info->type, "fill-extrusion") || !std::strcmp(info->type, "heatmap");
}

void CustomGeometrySource::setTileData(const CanonicalTileID& tileID,
                                     const GeoJSON& data) {
    loader->self().invoke(&CustomTileLoader::setTileData, tileID, data);
}

void CustomGeometrySource::invalidateTile(const CanonicalTileID& tileID) {
    loader->self().invoke(&CustomTileLoader::invalidateTile, tileID);
}

void CustomGeometrySource::invalidateRegion(const LatLngBounds& bounds) {
    loader->self().invoke(&CustomTileLoader::invalidateRegion, bounds, impl().getZoomRange());
}

} // namespace style
} // namespace mbgl
