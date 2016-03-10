#include <mbgl/annotation/point_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>

namespace mbgl {

PointAnnotationImpl::PointAnnotationImpl(const AnnotationID id_, const PointAnnotation& point_)
: id(id_),
  point(point_) {
}

void PointAnnotationImpl::updateLayer(const TileID& tileID, AnnotationTileLayer& layer) const {
    std::unordered_map<std::string, std::string> featureProperties;
    featureProperties.emplace("sprite", point.icon.empty() ? std::string("default_marker") : point.icon);

    mbgl::ScreenCoordinate projected = point.position.project();
    projected *= std::pow(2, tileID.z);
    projected.x = std::fmod(projected.x, 1);
    projected.y = std::fmod(projected.y, 1);
    projected *= GeometryTileFeature::defaultExtent;

    layer.features.emplace_back(
        std::make_shared<const AnnotationTileFeature>(FeatureType::Point,
                                                      GeometryCollection {{ {{ GeometryCoordinate { projected } }} }},
                                                      featureProperties));
}

} // namespace mbgl
