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

GeometryTile& GeometryTile::operator=(GeometryTile&& other) {
    if (this != &other) {
        layers.swap(other.layers);
    }
    return *this;
}

template <typename T>
GeometryFilteredTileLayer<T>::iterator::iterator(const GeometryFilteredTileLayer<T>& parent_)
    : parent(parent_) {
    operator++();
}
