#pragma once

#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geometry.hpp>

#include <string>
#include <memory>

namespace mbgl {

class AnnotationTileData;
class CanonicalTileID;

/**
 * Constains the geometry for this shape annotation and caches the
 * vector tile information created from it used to create render tiles.
 */
class ShapeAnnotationFeature {
public:
    ShapeAnnotationFeature(const AnnotationID&, ShapeAnnotationGeometry, const uint8_t maxZoom);
    ~ShapeAnnotationFeature() = default;

    void updateTileData(const CanonicalTileID&, AnnotationTileData&) const;

    const AnnotationID id;

private:
    const ShapeAnnotationGeometry geometry;
    const uint8_t maxZoom;
    const std::string layerID;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
};

} // namespace mbgl
