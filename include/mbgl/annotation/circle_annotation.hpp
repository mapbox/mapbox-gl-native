#ifndef MBGL_ANNOTATION_CIRCLE_ANNOTATION
#define MBGL_ANNOTATION_CIRCLE_ANNOTATION

#include <mbgl/util/geo.hpp>

#include <string>

namespace mbgl {

    class CircleAnnotation {
    public:
        inline CircleAnnotation(const LatLng& position_, const double radius_)
        : position(position_), radius(radius_) {
        }

        const LatLng position;
        const double radius;
    };
    
}

#endif
