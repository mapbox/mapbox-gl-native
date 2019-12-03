#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

#include <cstdint>
#include <vector>
#include <string>

namespace mbgl {

using AnnotationID = uint64_t;
using AnnotationIDs = std::vector<AnnotationID>;

class SymbolAnnotation {
public:
    SymbolAnnotation(Point<double> geometry_, std::string icon_ = {}) : geometry(geometry_), icon(std::move(icon_)) {}

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
    LineAnnotation(ShapeAnnotationGeometry geometry_,
                   style::PropertyValue<float> opacity_ = 1.0f,
                   style::PropertyValue<float> width_ = 1.0f,
                   style::PropertyValue<Color> color_ = Color::black())
        : geometry(std::move(geometry_)),
          opacity(std::move(opacity_)),
          width(std::move(width_)),
          color(std::move(color_)) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity;
    style::PropertyValue<float> width;
    style::PropertyValue<Color> color;
};

class FillAnnotation {
public:
    FillAnnotation(ShapeAnnotationGeometry geometry_,
                   style::PropertyValue<float> opacity_ = 1.0f,
                   style::PropertyValue<Color> color_ = Color::black(),
                   style::PropertyValue<Color> outlineColor_ = {})
        : geometry(std::move(geometry_)),
          opacity(std::move(opacity_)),
          color(std::move(color_)),
          outlineColor(std::move(outlineColor_)) {}

    ShapeAnnotationGeometry geometry;
    style::PropertyValue<float> opacity;
    style::PropertyValue<Color> color;
    style::PropertyValue<Color> outlineColor;
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation>;

} // namespace mbgl
