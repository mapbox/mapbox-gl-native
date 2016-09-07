#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

SymbolAnnotationImpl::SymbolAnnotationImpl(AnnotationID id_, SymbolAnnotation annotation_)
: id(id_),
  annotation(std::move(annotation_)) {
}

void SymbolAnnotationImpl::updateLayer(const CanonicalTileID& tileID, AnnotationTileLayer& layer) const {
    std::unordered_map<std::string, std::string> featureProperties;
    featureProperties.emplace("sprite", annotation.icon.empty() ? std::string("default_marker") : annotation.icon);

    const Point<double>& p = annotation.geometry;

    // Clamp to the latitude limits of Web Mercator.
    const double constrainedLatitude = util::clamp(p.y, -util::LATITUDE_MAX, util::LATITUDE_MAX);

    // Project a coordinate into unit space in a square map.
    const double sine = std::sin(constrainedLatitude * util::DEG2RAD);
    const double x = p.x / util::DEGREES_MAX + 0.5;
    const double y = 0.5 - 0.25 * std::log((1.0 + sine) / (1.0 - sine)) / M_PI;

    Point<double> projected(x, y);
    projected *= std::pow(2, tileID.z);
    projected.x = std::fmod(projected.x, 1);
    projected.y = std::fmod(projected.y, 1);
    projected *= double(util::EXTENT);

    layer.features.emplace_back(
        std::make_shared<const AnnotationTileFeature>(id,
                                                      FeatureType::Point,
                                                      GeometryCollection {{ {{ convertPoint<int16_t>(projected) }} }},
                                                      featureProperties));
}

} // namespace mbgl
