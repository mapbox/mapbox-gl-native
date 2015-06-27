#ifndef MBGL_ANNOTATION_POINT_ANNOTATION
#define MBGL_ANNOTATION_POINT_ANNOTATION

#include <mbgl/util/geo.hpp>

#include <string>

namespace mbgl {

class PointAnnotation {
public:
    inline PointAnnotation(const LatLng& position_, const std::string& icon_ = "")
        : position(position_), icon(icon_) {
    }

    const LatLng position;
    const std::string icon;
};

}

#endif
