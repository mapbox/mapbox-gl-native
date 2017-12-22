#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

using namespace style;
namespace geojsonvt = mapbox::geojsonvt;

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_)
    : id(id_),
      layerID(AnnotationManager::ShapeLayerID + util::toString(id)) {
}

void ShapeAnnotationImpl::updateTileData(const CanonicalTileID& tileID, AnnotationTileData& data) {
    static const double baseTolerance = 4;

    if (!shapeTiler) {
        mapbox::geometry::feature_collection<double> features;
        features.emplace_back(ShapeAnnotationGeometry::visit(geometry(), [] (auto&& geom) {
            return Feature { std::move(geom) };
        }));
        mapbox::geojsonvt::Options options;
        // The annotation source is currently hard coded to maxzoom 16, so we're topping out at z16
        // here as well.
        options.maxZoom = 16;
        options.buffer = 255u;
        options.extent = util::EXTENT;
        options.tolerance = baseTolerance;
        shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
    }

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (shapeTile.features.empty())
        return;

    auto layer = data.addLayer(layerID);

    ToGeometryCollection toGeometryCollection;
    ToFeatureType toFeatureType;
    for (const auto& shapeFeature : shapeTile.features) {
        FeatureType featureType = apply_visitor(toFeatureType, shapeFeature.geometry);
        GeometryCollection renderGeometry = apply_visitor(toGeometryCollection, shapeFeature.geometry);

        assert(featureType != FeatureType::Unknown);

        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (featureType == FeatureType::Polygon) {
            renderGeometry = fixupPolygons(renderGeometry);
        }

        layer->addFeature(id, featureType, renderGeometry);
    }
}

} // namespace mbgl
