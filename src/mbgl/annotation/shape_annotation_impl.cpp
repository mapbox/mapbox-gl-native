#include <mapbox/geojsonvt/convert.hpp>

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/fill_layer.hpp>

namespace mbgl {

namespace geojsonvt = mapbox::geojsonvt;

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_,
                                         const ShapeAnnotation& shape_,
                                         const uint8_t maxZoom_)
: id(id_),
  layerID("com.mapbox.annotations.shape." + util::toString(id)),
  shape(shape_),
  maxZoom(maxZoom_) {
}

void ShapeAnnotationImpl::updateStyle(Style& style) {
    if (style.getLayer(layerID))
        return;

    if (shape.properties.is<LineAnnotationProperties>()) {
        type = geojsonvt::ProjectedFeatureType::LineString;

        std::unique_ptr<LineLayer> layer = std::make_unique<LineLayer>();
        layer->layout.join = JoinType::Round;

        const LineAnnotationProperties& properties = shape.properties.get<LineAnnotationProperties>();
        layer->paint.opacity = properties.opacity;
        layer->paint.width = properties.width;
        layer->paint.color = properties.color;

        layer->id = layerID;
        layer->source = AnnotationManager::SourceID;
        layer->sourceLayer = layer->id;

        style.addLayer(std::move(layer), AnnotationManager::PointLayerID);

    } else if (shape.properties.is<FillAnnotationProperties>()) {
        type = geojsonvt::ProjectedFeatureType::Polygon;

        std::unique_ptr<FillLayer> layer = std::make_unique<FillLayer>();

        const FillAnnotationProperties& properties = shape.properties.get<FillAnnotationProperties>();
        layer->paint.opacity = properties.opacity;
        layer->paint.color = properties.color;
        layer->paint.outlineColor = properties.outlineColor;

        layer->id = layerID;
        layer->source = AnnotationManager::SourceID;
        layer->sourceLayer = layer->id;

        style.addLayer(std::move(layer), AnnotationManager::PointLayerID);

    } else {
        const StyleLayer* sourceLayer = style.getLayer(shape.properties.get<std::string>());
        if (!sourceLayer) return;

        std::unique_ptr<StyleLayer> layer = sourceLayer->clone();

        type = layer->is<LineLayer>()
            ? geojsonvt::ProjectedFeatureType::LineString
            : geojsonvt::ProjectedFeatureType::Polygon;

        layer->id = layerID;
        layer->ref = "";
        layer->source = AnnotationManager::SourceID;
        layer->sourceLayer = layer->id;
        layer->visibility = VisibilityType::Visible;

        style.addLayer(std::move(layer), sourceLayer->id);
    }
}

void ShapeAnnotationImpl::updateTile(const TileID& tileID, AnnotationTile& tile) {
    static const double baseTolerance = 4;

    if (!shapeTiler) {
        const uint64_t maxAmountOfTiles = 1 << maxZoom;
        const double tolerance = baseTolerance / (maxAmountOfTiles * GeometryTileFeature::defaultExtent);

        geojsonvt::ProjectedRings rings;
        std::vector<geojsonvt::LonLat> points;

        for (size_t i = 0; i < shape.segments[0].size(); ++i) { // first segment for now (no holes)
            const double constraintedLatitude = ::fmin(::fmax(shape.segments[0][i].latitude, -util::LATITUDE_MAX), util::LATITUDE_MAX);
            points.push_back(geojsonvt::LonLat(shape.segments[0][i].longitude, constraintedLatitude));
        }

        if (type == geojsonvt::ProjectedFeatureType::Polygon &&
                (points.front().lon != points.back().lon || points.front().lat != points.back().lat)) {
            points.push_back(geojsonvt::LonLat(points.front().lon, points.front().lat));
        }

        auto ring = geojsonvt::Convert::projectRing(points, tolerance);
        rings.push_back(ring);

        std::vector<geojsonvt::ProjectedFeature> features;
        features.push_back(geojsonvt::Convert::create(geojsonvt::Tags(), type, rings));

        mapbox::geojsonvt::Options options;
        options.maxZoom = maxZoom;
        options.buffer = 128u;
        options.tolerance = baseTolerance;
        shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
    }

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (!shapeTile)
        return;

    AnnotationTileLayer& layer = *tile.layers.emplace(layerID,
        std::make_unique<AnnotationTileLayer>()).first->second;

    for (auto& shapeFeature : shapeTile.features) {
        FeatureType featureType = FeatureType::Unknown;

        if (shapeFeature.type == geojsonvt::TileFeatureType::LineString) {
            featureType = FeatureType::LineString;
        } else if (shapeFeature.type == geojsonvt::TileFeatureType::Polygon) {
            featureType = FeatureType::Polygon;
        }

        assert(featureType != FeatureType::Unknown);

        GeometryCollection renderGeometry;
        for (auto& shapeRing : shapeFeature.tileGeometry.get<geojsonvt::TileRings>()) {
            GeometryCoordinates renderLine;

            for (auto& shapePoint : shapeRing) {
                renderLine.emplace_back(shapePoint.x, shapePoint.y);
            }

            renderGeometry.push_back(renderLine);
        }

        layer.features.emplace_back(
            std::make_shared<AnnotationTileFeature>(featureType, renderGeometry));
    }
}

} // namespace mbgl
