#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/custom_tile_loader.hpp>
#include <mbgl/style/sources/custom_geometry_source_impl.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <tuple>
#include <map>

namespace mbgl {
namespace style {

CustomGeometrySource::CustomGeometrySource(std::string id,
                                       const CustomGeometrySource::Options options)
    : Source(makeMutable<CustomGeometrySource::Impl>(std::move(id), options)),
    mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
    loader(std::make_unique<CustomTileLoader>(options.fetchTileFunction, options.cancelTileFunction)) {
}

CustomGeometrySource::~CustomGeometrySource() = default;

const CustomGeometrySource::Impl& CustomGeometrySource::impl() const {
    return static_cast<const CustomGeometrySource::Impl&>(*baseImpl);
}

void CustomGeometrySource::loadDescription(FileSource&) {
    baseImpl = makeMutable<CustomGeometrySource::Impl>(impl(), ActorRef<CustomTileLoader>(*loader, mailbox));
    loaded = true;
}

void CustomGeometrySource::setTileData(const CanonicalTileID& tileID,
                                     const GeoJSON& data) {
    loader->setTileData(tileID, data);
}

void CustomGeometrySource::invalidateTile(const CanonicalTileID& tileID) {
    loader->invalidateTile(tileID);
}

void CustomGeometrySource::invalidateRegion(const LatLngBounds& bounds) {
    loader->invalidateRegion(bounds, impl().getZoomRange());
}
    
} // namespace style
} // namespace mbgl
