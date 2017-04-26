#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace mbgl {

using AnnotationID = uint32_t;
using AnnotationIDs = std::vector<AnnotationID>;

class SymbolAnnotation {
public:
    Point<double> geometry;
    std::string icon;
};

using ShapeAnnotationGeometry = variant<
    LineString<double>,
    Polygon<double>,
    MultiLineString<double>,
    MultiPolygon<double>>;

class LineAnnotation {
public:
    ShapeAnnotationGeometry geometry;
    style::DataDrivenPropertyValue<float> opacity { 1.0f };
    style::PropertyValue<float> width { 1.0f };
    style::DataDrivenPropertyValue<Color> color { Color::black() };
};

class FillAnnotation {
public:
    ShapeAnnotationGeometry geometry;
    style::DataDrivenPropertyValue<float> opacity { 1.0f };
    style::DataDrivenPropertyValue<Color> color { Color::black() };
    style::DataDrivenPropertyValue<Color> outlineColor {};
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation>;

} // namespace mbgl
