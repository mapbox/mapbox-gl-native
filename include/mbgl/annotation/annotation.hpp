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
    LineAnnotation(const ShapeAnnotationGeometry &geometry_, const style::PropertyValue<float> &opacity_,
            const style::PropertyValue<float> &width_, const style::PropertyValue<Color> &color_) :
        geometry(geometry_), opacity(opacity_), width(width_), color(color_) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity { 1.0f };
    style::PropertyValue<float> width { 1.0f };
    style::PropertyValue<Color> color { Color::black() };
};

class FillAnnotation {
public:
    FillAnnotation(const ShapeAnnotationGeometry &geometry_, const style::PropertyValue<float> &opacity_,
            const style::PropertyValue<Color> &color_, const style::PropertyValue<Color> &outlineColor_) :
        geometry(geometry_), opacity(opacity_), color(color_), outlineColor(outlineColor_) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity { 1.0f };
    style::PropertyValue<Color> color { Color::black() };
    style::PropertyValue<Color> outlineColor {};
};

// An annotation whose type and properties are sourced from a style layer.
class StyleSourcedAnnotation {
public:
    ShapeAnnotationGeometry geometry;
    std::string layerID;
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation,
    StyleSourcedAnnotation>;

} // namespace mbgl
