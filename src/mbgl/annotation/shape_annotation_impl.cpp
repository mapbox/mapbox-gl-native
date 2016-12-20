#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

using namespace style;
namespace geojsonvt = mapbox::geojsonvt;

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_, const uint8_t maxZoom_)
    : id(id_),
      maxZoom(maxZoom_),
      layerID("com.mapbox.annotations.shape." + util::toString(id)) {
}

void ShapeAnnotationImpl::updateTileData(const CanonicalTileID& tileID, AnnotationTileData& data) {
    static const double baseTolerance = 4;

    if (!shapeTiler) {
        mapbox::geometry::feature_collection<double> features;
        features.emplace_back(ShapeAnnotationGeometry::visit(geometry(), [] (auto&& geom) {
#if !defined(__GNUC__) || __GNUC__ >= 5
            return Feature { std::move(geom) };
#else
            Feature feature;
            feature.geometry = std::move(geom);

            return feature;
#endif
        }));
        mapbox::geojsonvt::Options options;
        options.maxZoom = maxZoom;
        options.buffer = 255u;
        options.extent = util::EXTENT;
        options.tolerance = baseTolerance;
        shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
    }

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (shapeTile.features.empty())
        return;

    AnnotationTileLayer& layer = data.layers.emplace(layerID, layerID).first->second;

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

        layer.features.emplace_back(id, featureType, renderGeometry);
    }
}

} // namespace mbgl
