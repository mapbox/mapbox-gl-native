#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/custom_tile_loader.hpp>
#include <mbgl/actor/actor_ref.hpp>

namespace mbgl {
namespace style {

class CustomGeometrySource::Impl : public Source::Impl {
public:
    Impl(std::string id, CustomGeometrySource::Options options);
    Impl(const Impl&, ActorRef<CustomTileLoader>);

    optional<std::string> getAttribution() const final;

    CustomGeometrySource::TileOptions getTileOptions() const;
    Range<uint8_t> getZoomRange() const;
    optional<ActorRef<CustomTileLoader>> getTileLoader() const;

private:
    CustomGeometrySource::TileOptions tileOptions;
    Range<uint8_t> zoomRange;
    optional<ActorRef<CustomTileLoader>> loaderRef;
};

} // namespace style
} // namespace mbgl
