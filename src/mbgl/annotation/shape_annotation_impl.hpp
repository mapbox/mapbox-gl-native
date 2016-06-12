#pragma once

#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>

#include <string>
#include <memory>

namespace mbgl {

class AnnotationTileData;
class CanonicalTileID;

namespace style {
class Style;
} // namespace style

class ShapeAnnotationImpl {
public:
    ShapeAnnotationImpl(const AnnotationID, const uint8_t maxZoom);
    virtual ~ShapeAnnotationImpl() = default;

    virtual void updateStyle(style::Style&) const = 0;
    virtual const ShapeAnnotationGeometry& geometry() const = 0;

    void updateTileData(const CanonicalTileID&, AnnotationTileData&);

    const AnnotationID id;
    const uint8_t maxZoom;
    const std::string layerID;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
};

} // namespace mbgl
