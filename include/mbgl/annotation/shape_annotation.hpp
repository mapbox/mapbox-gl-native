#ifndef MBGL_ANNOTATION_SHAPE_ANNOTATION
#define MBGL_ANNOTATION_SHAPE_ANNOTATION

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/style_properties.hpp>

#include <mbgl/util/geo.hpp>

namespace mbgl {

using AnnotationSegment = std::vector<LatLng>;
using AnnotationSegments = std::vector<AnnotationSegment>;

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
