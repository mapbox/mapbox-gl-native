#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/sources/render_raster_dem_source.hpp>
#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/sources/render_custom_geometry_source.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

using namespace style;

std::unique_ptr<RenderSource> RenderSource::create(Immutable<Source::Impl> impl) {
    switch (impl->type) {
    case SourceType::Vector:
        return std::make_unique<RenderVectorSource>(staticImmutableCast<VectorSource::Impl>(impl));
    case SourceType::Raster:
        return std::make_unique<RenderRasterSource>(staticImmutableCast<RasterSource::Impl>(impl));
    case SourceType::RasterDEM:
        return std::make_unique<RenderRasterDEMSource>(staticImmutableCast<RasterSource::Impl>(impl));
    case SourceType::GeoJSON:
        return std::make_unique<RenderGeoJSONSource>(staticImmutableCast<GeoJSONSource::Impl>(impl));
    case SourceType::Video:
        assert(false);
        return nullptr;
    case SourceType::Annotations:
        return std::make_unique<RenderAnnotationSource>(staticImmutableCast<AnnotationSource::Impl>(impl));
    case SourceType::Image:
        return std::make_unique<RenderImageSource>(staticImmutableCast<ImageSource::Impl>(impl));
    case SourceType::CustomVector:
        return std::make_unique<RenderCustomGeometrySource>(staticImmutableCast<CustomGeometrySource::Impl>(impl));
    }

    // Not reachable, but placate GCC.
    assert(false);
    return nullptr;
}

static RenderSourceObserver nullObserver;

RenderSource::RenderSource(Immutable<style::Source::Impl> impl)
    : baseImpl(impl),
      observer(&nullObserver) {
}

void RenderSource::setObserver(RenderSourceObserver* observer_) {
    observer = observer_;
}

void RenderSource::onTileChanged(Tile& tile) {
    observer->onTileChanged(*this, tile.id);
}

void RenderSource::onTileError(Tile& tile, std::exception_ptr error) {
    observer->onTileError(*this, tile.id, error);
}

bool RenderSource::isEnabled() const {
    return enabled;
}

} // namespace mbgl
