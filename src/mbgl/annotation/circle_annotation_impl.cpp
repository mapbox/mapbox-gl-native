#include <mbgl/annotation/circle_annotation_impl.hpp>
//#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {

CircleAnnotationImpl::CircleAnnotationImpl(const AnnotationID id_, const CircleAnnotation& circle_)
: id(id_),
  layerID("com.mapbox.annotations.circle." + util::toString(id)),
  circle(circle_) {
}

void CircleAnnotationImpl::updateLayer(const TileID& tileID, AnnotationTileLayer& layer) const {
    std::unordered_map<std::string, std::string> featureProperties;
    featureProperties.emplace("sprite", point.icon.empty() ? std::string("default_marker") : point.icon);

    const uint16_t extent = 4096;
    const vec2<double> pp = point.position.project();
    const uint32_t z2 = 1 << tileID.z;
    const uint32_t x = pp.x * z2;
    const uint32_t y = pp.y * z2;
    const Coordinate coordinate(extent * (pp.x * z2 - x), extent * (pp.y * z2 - y));

    layer.features.emplace_back(
                                std::make_shared<const AnnotationTileFeature>(FeatureType::Point,
                                                                              GeometryCollection {{ {{ coordinate }} }},
                                                                              featureProperties));
}

LatLngBounds CircleAnnotationImpl::bounds() const {
    return LatLngBounds(circle.position, circle.position); //
}
    
}
