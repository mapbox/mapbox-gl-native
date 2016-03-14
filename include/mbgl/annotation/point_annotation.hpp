#ifndef MBGL_ANNOTATION_POINT_ANNOTATION
#define MBGL_ANNOTATION_POINT_ANNOTATION

#include <mbgl/util/geo.hpp>

#include <string>

namespace mbgl {

class PointAnnotation {
public:
    PointAnnotation(const LatLng& position_, const std::string& icon_ = "")
        : position(position_.wrapped()), icon(icon_) {}
    PointAnnotation(const LatLng& position_, const std::string& icon_ = "", const std::string& text_ = "")
            : position(position_), icon(icon_), text(text_) {}

    const LatLng position;
    const std::string icon;
    const std::string text;
};

} // namespace mbgl

#endif
