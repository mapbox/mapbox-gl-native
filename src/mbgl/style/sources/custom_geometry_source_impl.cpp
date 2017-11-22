#include <mbgl/style/sources/custom_geometry_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>

namespace mbgl {
namespace style {

CustomGeometrySource::Impl::Impl(std::string id_,
                               const CustomGeometrySource::Options options)
    : Source::Impl(SourceType::CustomVector, std::move(id_)),
      tileOptions(options.tileOptions),
      zoomRange(options.zoomRange),
      loaderRef({}) {
}

CustomGeometrySource::Impl::Impl(const Impl& impl, ActorRef<CustomTileLoader> loaderRef_)
    : Source::Impl(impl),
    tileOptions(impl.tileOptions),
    zoomRange(impl.zoomRange),
    loaderRef(loaderRef_){
    
}

optional<std::string> CustomGeometrySource::Impl::getAttribution() const {
    return {};
}

CustomGeometrySource::TileOptions CustomGeometrySource::Impl::getTileOptions() const {
    return tileOptions;
}

Range<uint8_t> CustomGeometrySource::Impl::getZoomRange() const {
    return zoomRange;
}

optional<ActorRef<CustomTileLoader>> CustomGeometrySource::Impl::getTileLoader() const {
    return loaderRef;
}

} // namespace style
} // namespace mbgl
