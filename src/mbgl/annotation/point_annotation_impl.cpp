#include <mbgl/annotation/point_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

PointAnnotationImpl::PointAnnotationImpl(const AnnotationID id_, const PointAnnotation& point_)
: id(id_),
  point(point_) {
}

void PointAnnotationImpl::updateLayer(const TileID& tileID, AnnotationTileLayer& layer) const {
    std::unordered_map<std::string, std::string> featureProperties;
    featureProperties.emplace("sprite", point.icon.empty() ? std::string("default_marker") : point.icon);

    // Clamp to the latitude limits of Web Mercator.
    const double constrainedLatitude = util::clamp(point.position.latitude, -util::LATITUDE_MAX, util::LATITUDE_MAX);

    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(constrainedLatitude * util::DEG2RAD);
    const double x = point.position.longitude / util::DEGREES_MAX + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;

    Point<double> projected(x, y);
    projected *= std::pow(2, tileID.z);
    projected.x = std::fmod(projected.x, 1);
    projected.y = std::fmod(projected.y, 1);
    projected *= double(GeometryTileFeature::defaultExtent);

    layer.features.emplace_back(
        std::make_shared<const AnnotationTileFeature>(FeatureType::Point,
                                                      GeometryCollection {{ {{ convertPoint<int16_t>(projected) }} }},
                                                      featureProperties));
}

} // namespace mbgl
