#pragma once

#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geometry.hpp>

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

struct CloseShapeAnnotation {
    ShapeAnnotationGeometry operator()(const mbgl::LineString<double> &geom) const {
        return geom;
    }
    ShapeAnnotationGeometry operator()(const mbgl::MultiLineString<double> &geom) const {
        return geom;
    }
    ShapeAnnotationGeometry operator()(const mbgl::Polygon<double> &geom) const {
        mbgl::Polygon<double> closed = geom;
        for (auto &ring : closed) {
            if (!ring.empty() && ring.front() != ring.back()) {
                ring.emplace_back(ring.front());
            }
        }
        return closed;
    }
    ShapeAnnotationGeometry operator()(const mbgl::MultiPolygon<double> &geom) const {
        mbgl::MultiPolygon<double> closed = geom;
        for (auto &polygon : closed) {
            for (auto &ring : polygon) {
                if (!ring.empty() && ring.front() != ring.back()) {
                    ring.emplace_back(ring.front());
                }
            }
        }
        return closed;
    }
};

} // namespace mbgl
