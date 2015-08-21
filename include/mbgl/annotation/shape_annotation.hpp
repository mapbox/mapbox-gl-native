#ifndef MBGL_ANNOTATION_SHAPE_ANNOTATION
#define MBGL_ANNOTATION_SHAPE_ANNOTATION

#include <mbgl/util/geo.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style_properties.hpp>

#include <string>

namespace mbgl {

class ShapeAnnotation {
public:
    inline ShapeAnnotation(const AnnotationSegments& segments_, const StyleProperties& styleProperties_)
        : segments(segments_), styleProperties(styleProperties_) {
    }

    const AnnotationSegments segments;
    const StyleProperties styleProperties;
};

}

#endif
