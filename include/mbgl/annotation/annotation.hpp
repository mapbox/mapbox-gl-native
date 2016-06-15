#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>

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
    float opacity = 1;
    float width = 1;
    Color color = { 0, 0, 0, 1 };
};

class FillAnnotation {
public:
    ShapeAnnotationGeometry geometry;
    float opacity = 1;
    Color color = { 0, 0, 0, 1 };
    Color outlineColor = { 0, 0, 0, -1 };
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
