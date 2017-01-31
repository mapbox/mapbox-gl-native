#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace mbgl {

using AnnotationID = uint32_t;
using AnnotationIDs = std::vector<AnnotationID>;

class SymbolAnnotation {
public:
    SymbolAnnotation() = default;

    SymbolAnnotation(Point<double> geometry_, std::string icon_)
        : geometry(std::move(geometry_)), icon(std::move(icon_)) {}

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
    LineAnnotation(ShapeAnnotationGeometry geometry_, style::PropertyValue<float> opacity_,
                   style::PropertyValue<float> width_, style::PropertyValue<Color> color_)
        : geometry(std::move(geometry_)), opacity(std::move(opacity_)), width(std::move(width_)), color(std::move(color_)) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity { 1.0f };
    style::PropertyValue<float> width { 1.0f };
    style::PropertyValue<Color> color { Color::black() };
};

class FillAnnotation {
public:
    FillAnnotation(ShapeAnnotationGeometry geometry_, style::PropertyValue<float> opacity_,
                   style::PropertyValue<Color> color_, style::PropertyValue<Color> outlineColor_)
        : geometry(std::move(geometry_)), opacity(std::move(opacity_)), color(std::move(color_)), outlineColor(std::move(outlineColor_)) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity { 1.0f };
    style::PropertyValue<Color> color { Color::black() };
    style::PropertyValue<Color> outlineColor {};
};

// An annotation whose type and properties are sourced from a style layer.
class StyleSourcedAnnotation {
public:
    StyleSourcedAnnotation(ShapeAnnotationGeometry geometry_, std::string layerID_)
        : geometry(std::move(geometry_)), layerID(std::move(layerID_)) {}

    ShapeAnnotationGeometry geometry;
    std::string layerID;
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation,
    StyleSourcedAnnotation>;

} // namespace mbgl
