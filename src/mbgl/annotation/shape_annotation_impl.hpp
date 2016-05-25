#pragma once

#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/util/geo.hpp>

#include <memory>
#include <string>
#include <map>

namespace mbgl {

class Style;
class AnnotationTile;
class CanonicalTileID;

class ShapeAnnotationImpl {
public:
    using Map = std::map<AnnotationID, std::unique_ptr<ShapeAnnotationImpl>>;

    ShapeAnnotationImpl(const AnnotationID, const ShapeAnnotation&, const uint8_t maxZoom);

    void updateStyle(Style&);
    void updateTile(const CanonicalTileID&, AnnotationTile&);

    const AnnotationID id;
    const std::string layerID;
    const ShapeAnnotation shape;

private:
    const uint8_t maxZoom;
    mapbox::geojsonvt::ProjectedFeatureType type;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
};

} // namespace mbgl
