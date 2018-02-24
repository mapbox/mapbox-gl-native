#include <mbgl/tile/custom_geometry_tile.hpp>
#include <mbgl/tile/geojson_tile_data.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/style/custom_tile_loader.hpp>

#include <mapbox/geojsonvt.hpp>

namespace mbgl {

CustomGeometryTile::CustomGeometryTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const TileParameters& parameters,
                         const style::CustomGeometrySource::TileOptions options_,
                         ActorRef<style::CustomTileLoader> loader_)
    : GeometryTile(overscaledTileID, sourceID_, parameters),
    necessity(TileNecessity::Optional),
    options(options_),
    loader(loader_),
    mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent())),
    actorRef(*this, mailbox) {
}

CustomGeometryTile::~CustomGeometryTile() {
    loader.invoke(&style::CustomTileLoader::removeTile, id);
}

void CustomGeometryTile::setTileData(const GeoJSON& geoJSON) {

    auto featureData = mapbox::geometry::feature_collection<int16_t>();
    if (geoJSON.is<FeatureCollection>() && !geoJSON.get<FeatureCollection>().empty()) {
        const double scale = util::EXTENT / options.tileSize;

        mapbox::geojsonvt::TileOptions vtOptions;
        vtOptions.extent = util::EXTENT;
        vtOptions.buffer = ::round(scale * options.buffer);
        vtOptions.tolerance = scale * options.tolerance;
        featureData = mapbox::geojsonvt::geoJSONToTile(geoJSON, id.canonical.z, id.canonical.x, id.canonical.y, vtOptions, options.wrap, options.clip).features;
    } else {
        setNecessity(TileNecessity::Optional);
    }
    setData(std::make_unique<GeoJSONTileData>(std::move(featureData)));
}

void CustomGeometryTile::invalidateTileData() {
    stale = true;
    observer->onTileChanged(*this);
}

//Fetching tile data for custom sources is assumed to be an expensive operation.
// Only required tiles make fetchTile requests. Attempt to cancel a tile
// that is no longer required.
void CustomGeometryTile::setNecessity(TileNecessity newNecessity) {
   if (newNecessity != necessity || stale ) {
        necessity = newNecessity;
        if (necessity == TileNecessity::Required) {
            loader.invoke(&style::CustomTileLoader::fetchTile, id, actorRef);
            stale = false;
        } else if (!isRenderable()) {
            loader.invoke(&style::CustomTileLoader::cancelTile, id);
        }
    }
}

void CustomGeometryTile::querySourceFeatures(
    std::vector<Feature>& result,
    const SourceQueryOptions& queryOptions) {

    // Ignore the sourceLayer, there is only one
    auto layer = getData()->getLayer({});

    if (layer) {
        auto featureCount = layer->featureCount();
        for (std::size_t i = 0; i < featureCount; i++) {
            auto feature = layer->getFeature(i);
            
            // Apply filter, if any
            if (queryOptions.filter && !(*queryOptions.filter)(*feature)) {
                continue;
            }

            result.push_back(convertFeature(*feature, id.canonical));
        }
    }
}

} // namespace mbgl
