#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/style/filter_expression_private.hpp>

#include <iostream>

using namespace mbgl;

std::ostream& mbgl::operator<<(std::ostream& os, const GeometryFeatureType& type) {
    switch (type) {
        case GeometryFeatureType::Unknown: return os << "Unknown";
        case GeometryFeatureType::Point: return os << "Point";
        case GeometryFeatureType::LineString: return os << "LineString";
        case GeometryFeatureType::Polygon: return os << "Polygon";
        default: return os << "Invalid";
    }
}

std::ostream& mbgl::operator<<(std::ostream& os, const GeometryTileFeature& feature) {
    os << "Feature(" << feature.id << "): " << feature.type << std::endl;
    for (const auto& prop : feature.properties) {
        os << "  - " << prop.first << ": " << &prop.second << std::endl;
    }
    return os;
}

GeometryFilteredTileLayer::GeometryFilteredTileLayer(const util::ptr<GeometryTileLayer> layer_, const FilterExpression& filterExpression_)
    : layer(layer_),
      filterExpression(filterExpression_) {}

GeometryTile& GeometryTile::operator=(GeometryTile&& other) {
    if (this != &other) {
        layers.swap(other.layers);
    }
    return *this;
}
