#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/tile/geojson_tile_data.hpp>
namespace mbgl {

GeoJSONTile::GeoJSONTile(const OverscaledTileID& overscaledTileID,
                         std::string sourceID_,
                         const TileParameters& parameters,
                         std::shared_ptr<style::GeoJSONData> data_)
    : GeometryTile(overscaledTileID, sourceID_, parameters) {
    updateData(std::move(data_), false /*needsRelayout*/);
}

void GeoJSONTile::updateData(std::shared_ptr<style::GeoJSONData> data_, bool needsRelayout) {
    assert(data_);
    data = std::move(data_);
    if (needsRelayout) reset();
    data->getTile(
        id.canonical,
        [this, self = weakFactory.makeWeakPtr(), capturedData = data.get()](style::GeoJSONData::TileFeatures features) {
            if (!self) return;
            if (data.get() != capturedData) return;
            auto tileData = std::make_unique<GeoJSONTileData>(std::move(features));
            setData(std::move(tileData));
        });
}

void GeoJSONTile::querySourceFeatures(
    std::vector<Feature>& result,
    const SourceQueryOptions& options) {

    // Ignore the sourceLayer, there is only one
    if (auto tileData = getData()) {
        if (auto layer = tileData->getLayer({})) {
            auto featureCount = layer->featureCount();
            for (std::size_t i = 0; i < featureCount; i++) {
                auto feature = layer->getFeature(i);

                // Apply filter, if any
                if (options.filter && !(*options.filter)(style::expression::EvaluationContext { static_cast<float>(this->id.overscaledZ), feature.get() })) {
                    continue;
                }

                result.push_back(convertFeature(*feature, id.canonical));
            }
        }
    }
}

} // namespace mbgl
