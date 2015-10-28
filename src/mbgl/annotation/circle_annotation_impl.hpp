#ifndef MBGL_CIRCLE_ANNOTATION_IMPL
#define MBGL_CIRCLE_ANNOTATION_IMPL

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/circle_annotation.hpp>
#include <mbgl/util/geo.hpp>

#include <string>

namespace mbgl {

class AnnotationTileLayer;

class CircleAnnotationImpl {
public:
    CircleAnnotationImpl(const AnnotationID, const CircleAnnotation&);

    LatLngBounds bounds() const;
    void updateStyle(Style&);
    void updateLayer(const TileID&, AnnotationTile&) const;

    const AnnotationID id;
    const std::string layerID;
    const CircleAnnotation circle;
};

}

#endif
