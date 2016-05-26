#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

struct FillAnnotationProperties {
    float opacity = 1;
    Color color = {{ 0, 0, 0, 1 }};
    Color outlineColor = {{ 0, 0, 0, -1 }};
};

struct LineAnnotationProperties {
    float opacity = 1;
    float width = 1;
    Color color = {{ 0, 0, 0, 1 }};
};

class ShapeAnnotation {
public:
    using Properties = variant<
        FillAnnotationProperties, // creates a fill annotation
        LineAnnotationProperties, // creates a line annotation
        std::string>; // creates an annotation whose type and properties are sourced from a style layer

    ShapeAnnotation(const Geometry<double>& geometry_, const Properties& properties_)
        : geometry(geometry_), properties(properties_) {}

    const Geometry<double> geometry;
    const Properties properties;
};

} // namespace mbgl
