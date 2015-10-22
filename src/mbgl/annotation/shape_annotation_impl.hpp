#ifndef MBGL_SHAPE_ANNOTATION_IMPL
#define MBGL_SHAPE_ANNOTATION_IMPL

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geojsonvt/geojsonvt.hpp>

#include <memory>
#include <string>
#include <map>

namespace mbgl {

class Style;
class StyleLayer;
class AnnotationTile;

class ShapeAnnotationImpl {
public:
    using Map = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;

    ShapeAnnotationImpl(const AnnotationID, const ShapeAnnotation&, const uint8_t maxZoom);

    LatLngBounds bounds() const;
    void updateStyle(Style&);
    void updateTile(const TileID&, AnnotationTile&);

    const AnnotationID id;
    const std::string layerID;
    const ShapeAnnotation shape;

private:
    std::unique_ptr<StyleLayer> createLineLayer();
    std::unique_ptr<StyleLayer> createFillLayer();

    const uint8_t maxZoom;
    mapbox::util::geojsonvt::ProjectedFeatureType type;
    std::unique_ptr<mapbox::util::geojsonvt::GeoJSONVT> shapeTiler;
};

}

#endif
