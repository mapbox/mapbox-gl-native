#include <mapbox/geojsonvt/convert.hpp>

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

using namespace style;
namespace geojsonvt = mapbox::geojsonvt;

ShapeAnnotationImpl::ShapeAnnotationImpl(const AnnotationID id_, const uint8_t maxZoom_)
    : id(id_),
      maxZoom(maxZoom_),
      layerID("com.mapbox.annotations.shape." + util::toString(id)) {
}

struct ToGeoJSONVT {
    const double tolerance;

    ToGeoJSONVT(const double tolerance_)
        : tolerance(tolerance_) {
    }

    geojsonvt::ProjectedFeature operator()(const LineString<double>& line) const {
        geojsonvt::ProjectedRings converted;
        converted.push_back(convertPoints(geojsonvt::ProjectedFeatureType::LineString, line));
        return convertFeature(geojsonvt::ProjectedFeatureType::LineString, converted);
    }

    geojsonvt::ProjectedFeature operator()(const Polygon<double>& polygon) const {
        geojsonvt::ProjectedRings converted;
        for (const auto& ring : polygon) {
            converted.push_back(convertPoints(geojsonvt::ProjectedFeatureType::Polygon, ring));
        }
        return convertFeature(geojsonvt::ProjectedFeatureType::Polygon, converted);
    }

    geojsonvt::ProjectedFeature operator()(const MultiLineString<double>& lines) const {
        geojsonvt::ProjectedRings converted;
        for (const auto& line : lines) {
            converted.push_back(convertPoints(geojsonvt::ProjectedFeatureType::LineString, line));
        }
        return convertFeature(geojsonvt::ProjectedFeatureType::LineString, converted);
    }

    geojsonvt::ProjectedFeature operator()(const MultiPolygon<double>& polygons) const {
        geojsonvt::ProjectedRings converted;
        for (const auto& polygon : polygons) {
            for (const auto& ring : polygon) {
                converted.push_back(convertPoints(geojsonvt::ProjectedFeatureType::Polygon, ring));
            }
        }
        return convertFeature(geojsonvt::ProjectedFeatureType::Polygon, converted);
    }

private:
    geojsonvt::LonLat convertPoint(const Point<double>& p) const {
        return {
            util::wrap(p.x, -util::LONGITUDE_MAX, util::LONGITUDE_MAX),
            util::clamp(p.y, -util::LATITUDE_MAX, util::LATITUDE_MAX)
        };
    }

    geojsonvt::ProjectedRing convertPoints(geojsonvt::ProjectedFeatureType type, const std::vector<Point<double>>& points) const {
        std::vector<geojsonvt::LonLat> converted;

        for (const auto& p : points) {
            converted.push_back(convertPoint(p));
        }

        assert(points.size() > 0);
        if (type == geojsonvt::ProjectedFeatureType::Polygon && points.size() > 0 && points.front() != points.back()) {
            converted.push_back(converted.front());
        }

        return geojsonvt::Convert::projectRing(converted, tolerance);
    }

    geojsonvt::ProjectedFeature convertFeature(geojsonvt::ProjectedFeatureType type, const geojsonvt::ProjectedRings& rings) const {
        return geojsonvt::Convert::create(geojsonvt::Tags(), type, rings);
    }
};

void ShapeAnnotationImpl::updateTileData(const CanonicalTileID& tileID, AnnotationTileData& data) {
    static const double baseTolerance = 4;

    if (!shapeTiler) {
        const uint64_t maxAmountOfTiles = 1 << maxZoom;
        const double tolerance = baseTolerance / (maxAmountOfTiles * util::EXTENT);

        std::vector<geojsonvt::ProjectedFeature> features = {
            ShapeAnnotationGeometry::visit(geometry(), ToGeoJSONVT(tolerance))
        };

        mapbox::geojsonvt::Options options;
        options.maxZoom = maxZoom;
        options.buffer = 255u;
        options.extent = util::EXTENT;
        options.tolerance = baseTolerance;
        shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
    }

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (!shapeTile)
        return;

    AnnotationTileLayer& layer = *data.layers.emplace(layerID,
        std::make_unique<AnnotationTileLayer>(layerID)).first->second;

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

        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (featureType == FeatureType::Polygon) {
            renderGeometry = fixupPolygons(renderGeometry);
        }

        layer.features.emplace_back(
            std::make_shared<AnnotationTileFeature>(featureType, renderGeometry));
    }
}

} // namespace mbgl
