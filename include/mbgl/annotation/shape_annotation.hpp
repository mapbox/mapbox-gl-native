#ifndef MBGL_ANNOTATION_SHAPE_ANNOTATION
#define MBGL_ANNOTATION_SHAPE_ANNOTATION

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/geo.hpp>

#include <mapbox/variant.hpp>

namespace mbgl {

using AnnotationSegment = std::vector<LatLng>;
using AnnotationSegments = std::vector<AnnotationSegment>;

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
    using Properties = mapbox::util::variant<
        FillAnnotationProperties, // creates a fill annotation
        LineAnnotationProperties, // creates a line annotation
        std::string>; // creates an annotation whose type and properties are sourced from a style layer

    ShapeAnnotation(const AnnotationSegments& segments_, const Properties& properties_)
        : segments(segments_), properties(properties_) {
    }

    const AnnotationSegments segments;
    const Properties properties;
};

}

#endif
