#include <mbgl/style/sources/custom_vector_source_impl.hpp>
#include <mbgl/tile/vector_tile.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mapbox/geojsonvt.hpp>
#include <supercluster.hpp>

namespace mbgl {
namespace style {
    
CustomVectorSource::Impl::Impl(std::string id, Source& base_, GeoJSONOptions options_, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile_)
    : Source::Impl(SourceType::Vector, std::move(id), base_), options(options_), fetchTile(fetchTile_) {
    loaded = true;
}
    
Range<uint8_t> CustomVectorSource::Impl::getZoomRange() {
    return { options.minzoom, options.maxzoom };
}
    
uint16_t CustomVectorSource::Impl::getTileSize() const {
    return options.tileSize;
}
    
std::unique_ptr<Tile> CustomVectorSource::Impl::createTile(const OverscaledTileID& tileID,
                                                          const UpdateParameters& parameters) {
    auto tilePointer = std::make_unique<GeoJSONTile>(tileID, base.getID(), parameters);
    fetchTile(tileID.canonical.z, tileID.canonical.x, tileID.canonical.y);
    return std::move(tilePointer);
}
    
void CustomVectorSource::Impl::setTileData(uint8_t z, uint32_t x, uint32_t y, const mapbox::geojson::geojson& geoJSON) {
    double scale = util::EXTENT / util::tileSize;
    
    variant<GeoJSONVTPointer, SuperclusterPointer> geoJSONOrSupercluster;
    if (!options.cluster) {
        mapbox::geojsonvt::Options vtOptions;
        vtOptions.maxZoom = options.maxzoom;
        vtOptions.extent = util::EXTENT;
        vtOptions.buffer = std::round(scale * options.buffer);
        vtOptions.tolerance = scale * options.tolerance;
        geoJSONOrSupercluster = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(geoJSON, vtOptions);
    } else {
        mapbox::supercluster::Options clusterOptions;
        clusterOptions.maxZoom = options.clusterMaxZoom;
        clusterOptions.extent = util::EXTENT;
        clusterOptions.radius = std::round(scale * options.clusterRadius);
        
        const auto& features = geoJSON.get<mapbox::geometry::feature_collection<double>>();
        geoJSONOrSupercluster =
        std::make_unique<mapbox::supercluster::Supercluster>(features, clusterOptions);
    }
    
    for (auto const &item : tiles) {
        GeoJSONTile* tile = static_cast<GeoJSONTile*>(item.second.get());
        if(tile->id.canonical.z == z && tile->id.canonical.x == x && tile->id.canonical.y == y) {
            if (geoJSONOrSupercluster.is<GeoJSONVTPointer>()) {
                tile->updateData(geoJSONOrSupercluster.get<GeoJSONVTPointer>()->getTile(z, x, y).features);
            } else {
                assert(geoJSONOrSupercluster.is<SuperclusterPointer>());
                tile->updateData(geoJSONOrSupercluster.get<SuperclusterPointer>()->getTile(z, x, y));
            }
        }
    }
}
    
void CustomVectorSource::Impl::updateTile(uint8_t z, uint32_t x, uint32_t y) {
    for (auto const &item : tiles) {
        GeoJSONTile* tile = static_cast<GeoJSONTile*>(item.second.get());
        if(tile->id.canonical.z == z && tile->id.canonical.x == x && tile->id.canonical.y == y) {
            fetchTile(z, x, y);
        }
    }
}
    
} // namespace style
} // namespace mbgl
