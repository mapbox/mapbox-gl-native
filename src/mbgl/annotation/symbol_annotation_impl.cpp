#include <mbgl/annotation/symbol_annotation_impl.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/tile_coordinate.hpp>

namespace mbgl {

SymbolAnnotationImpl::SymbolAnnotationImpl(AnnotationID id_, SymbolAnnotation annotation_)
: id(id_),
  annotation(std::move(annotation_)) {
}

void SymbolAnnotationImpl::updateLayer(const CanonicalTileID& tileID, AnnotationTileLayer& layer) const {
    std::unordered_map<std::string, std::string> featureProperties;
    featureProperties.emplace("sprite", annotation.icon.empty() ? std::string("default_marker") : annotation.icon);

    LatLng latLng { annotation.geometry.y, annotation.geometry.x };
    TileCoordinate coordinate = TileCoordinate::fromLatLng(0, latLng);
    GeometryCoordinate tilePoint = TileCoordinate::toGeometryCoordinate(UnwrappedTileID(0, tileID), coordinate.p);
    layer.addFeature(id, FeatureType::Point, GeometryCollection {{ {{ tilePoint }} }}, featureProperties);
}

} // namespace mbgl
