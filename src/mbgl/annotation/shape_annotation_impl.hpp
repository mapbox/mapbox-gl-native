#ifndef MBGL_SHAPE_ANNOTATION_IMPL
#define MBGL_SHAPE_ANNOTATION_IMPL

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojsonvt/geojsonvt.hpp>

#include <string>
#include <map>

namespace mbgl {

class Style;
class AnnotationTile;

class ShapeAnnotationImpl {
public:
    using Map = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;

    ShapeAnnotationImpl(const AnnotationID, const ShapeAnnotation&, const uint8_t maxZoom);

    LatLngBounds bounds() const;
    void updateStyle(Style&);
    void updateTile(const TileID&, AnnotationTile&);

private:
    const AnnotationID id;
    const std::string layerID;
    const ShapeAnnotation shape;
    mapbox::util::geojsonvt::GeoJSONVT shapeTiler;
};

}

#endif
