#include <mbgl/annotation/shape_annotation_feature.hpp>
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

/**
 * Ensures (multi-)polygons are properly closed
 */
struct CloseShapeAnnotation {
    ShapeAnnotationGeometry operator()(const mbgl::LineString<double>& geom) const {
        return geom;
    }

    ShapeAnnotationGeometry operator()(const mbgl::MultiLineString<double>& geom) const {
        return geom;
    }

    ShapeAnnotationGeometry operator()(const mbgl::Polygon<double>& geom) const {
        mbgl::Polygon<double> closed = geom;
        for (auto& ring : closed) {
            if (!ring.empty() && ring.front() != ring.back()) {
                ring.emplace_back(ring.front());
            }
        }
        return closed;
    }

    ShapeAnnotationGeometry operator()(const mbgl::MultiPolygon<double>& geom) const {
        mbgl::MultiPolygon<double> closed = geom;
        for (auto& polygon : closed) {
            for (auto& ring : polygon) {
                if (!ring.empty() && ring.front() != ring.back()) {
                    ring.emplace_back(ring.front());
                }
            }
        }
        return closed;
    }
};

ShapeAnnotationFeature::ShapeAnnotationFeature(const AnnotationID& id_,
                                               ShapeAnnotationGeometry geometry_,
                                               const uint8_t maxZoom_)
        : id(id_), geometry(std::move(geometry_)), maxZoom(maxZoom_),
          layerID("com.mapbox.annotations.shape." + util::toString(id)) {

    static const double baseTolerance = 4;

    // Close geometry rings properly
    ShapeAnnotationGeometry closed = ShapeAnnotationGeometry::visit(geometry,
                                                                    CloseShapeAnnotation {});

    // Convert to feature collection
    mapbox::geometry::feature_collection<double> features;
    features.emplace_back(ShapeAnnotationGeometry::visit(closed, [](auto&& geom) {
        return Feature {std::move(geom)};
    }));

    // Create vector tiler
    mapbox::geojsonvt::Options options;
    options.maxZoom = maxZoom;
    options.buffer = 255u;
    options.extent = util::EXTENT;
    options.tolerance = baseTolerance;
    shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
}

void ShapeAnnotationFeature::updateTileData(const CanonicalTileID& tileID,
                                            AnnotationTileData& data) const {

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (shapeTile.features.empty()) {
        return;
    }

    AnnotationTileLayer& layer = data.layers.emplace(layerID, layerID).first->second;

    ToGeometryCollection toGeometryCollection;
    ToFeatureType toFeatureType;
    for (const auto& shapeFeature : shapeTile.features) {
        FeatureType featureType = apply_visitor(toFeatureType, shapeFeature.geometry);
        GeometryCollection renderGeometry = apply_visitor(toGeometryCollection,
                                                          shapeFeature.geometry);

        assert(featureType != FeatureType::Unknown);

        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (featureType == FeatureType::Polygon) {
            renderGeometry = fixupPolygons(renderGeometry);
        }

        layer.features.emplace_back(id, featureType, renderGeometry);
    }
}

} // namespace mbgl
