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
    SymbolAnnotation(Point<double> geometry_, std::string icon_ = {})
        : geometry(std::move(geometry_)),
          icon(std::move(icon_)) {}

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
                   style::DataDrivenPropertyValue<float> opacity_ = 1.0f,
                   style::DataDrivenPropertyValue<float> width_ = 1.0f,
                   style::DataDrivenPropertyValue<Color> color_ = Color::black())
        : geometry(std::move(geometry_)),
          opacity(std::move(opacity_)),
          width(std::move(width_)),
          color(std::move(color_)) {}

    ShapeAnnotationGeometry geometry;
    style::DataDrivenPropertyValue<float> opacity;
    style::DataDrivenPropertyValue<float> width;
    style::DataDrivenPropertyValue<Color> color;
};

class FillAnnotation {
public:
    FillAnnotation(ShapeAnnotationGeometry geometry_,
                   style::DataDrivenPropertyValue<float> opacity_ = 1.0f,
                   style::DataDrivenPropertyValue<Color> color_ = Color::black(),
                   style::DataDrivenPropertyValue<Color> outlineColor_ = {})
        : geometry(std::move(geometry_)),
          opacity(std::move(opacity_)),
          color(std::move(color_)),
          outlineColor(std::move(outlineColor_)) {}

    ShapeAnnotationGeometry geometry;
    style::DataDrivenPropertyValue<float> opacity;
    style::DataDrivenPropertyValue<Color> color;
    style::DataDrivenPropertyValue<Color> outlineColor;
};

using Annotation = variant<
    SymbolAnnotation,
    LineAnnotation,
    FillAnnotation>;

} // namespace mbgl
