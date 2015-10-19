#ifndef MBGL_ANNOTATION_SHAPE_ANNOTATION
#define MBGL_ANNOTATION_SHAPE_ANNOTATION

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/style/style_properties.hpp>

#include <mbgl/util/geo.hpp>

#include <mapbox/variant.hpp>

namespace mbgl {

using AnnotationSegment = std::vector<LatLng>;
using AnnotationSegments = std::vector<AnnotationSegment>;

class ShapeAnnotation {
public:
    using Properties = mapbox::util::variant<FillPaintProperties, LinePaintProperties>;

    inline ShapeAnnotation(const AnnotationSegments& segments_, const Properties& styleProperties_)
        : segments(segments_), styleProperties(styleProperties_) {
    }

    const AnnotationSegments segments;
    const Properties styleProperties;
};

}

#endif
