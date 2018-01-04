#pragma once

#include <mbgl/util/string.hpp>
#include <mapbox/geojsonvt.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/style.hpp>

#include <string>
#include <memory>

namespace mbgl {

class AnnotationTileData;
class CanonicalTileID;

class ShapeAnnotationImpl {
public:
    ShapeAnnotationImpl(const AnnotationID);
    virtual ~ShapeAnnotationImpl() = default;

    virtual void updateStyle(style::Style::Impl&) const = 0;
    virtual const ShapeAnnotationGeometry& geometry() const = 0;

    void updateTileData(const CanonicalTileID&, AnnotationTileData&);

    const AnnotationID id;
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
