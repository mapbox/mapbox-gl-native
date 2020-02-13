#include <mbgl/style/sources/custom_geometry_source_impl.hpp>
#include <mbgl/style/source_observer.hpp>

namespace mbgl {
namespace style {

CustomGeometrySource::Impl::Impl(std::string id_, CustomGeometrySource::Options options)
    : Source::Impl(SourceType::CustomVector, std::move(id_)),
      tileOptions(makeMutable<CustomGeometrySource::TileOptions>(options.tileOptions)),
      zoomRange(options.zoomRange),
      loaderRef({}) {}

CustomGeometrySource::Impl::Impl(const Impl& impl, ActorRef<CustomTileLoader> loaderRef_)
    : Source::Impl(impl), tileOptions(impl.tileOptions), zoomRange(impl.zoomRange), loaderRef(loaderRef_) {}

bool CustomGeometrySource::Impl::operator!=(const Impl& other) const noexcept {
    return tileOptions != other.tileOptions || zoomRange != other.zoomRange || bool(loaderRef) != bool(other.loaderRef);
}

optional<std::string> CustomGeometrySource::Impl::getAttribution() const {
    return {};
}

Immutable<CustomGeometrySource::TileOptions> CustomGeometrySource::Impl::getTileOptions() const {
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
