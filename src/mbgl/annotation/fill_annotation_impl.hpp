#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class FillAnnotationImpl : public ShapeAnnotationImpl {
public:
    FillAnnotationImpl(AnnotationID, FillAnnotation, uint8_t maxZoom);
    void updateStyle(style::Style&) const final;
    const ShapeAnnotationGeometry& geometry() const final;

private:
    const FillAnnotation annotation;
};

struct ToClosed {
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

const ShapeAnnotationGeometry ensureClosed(const ShapeAnnotationGeometry&);

} // namespace mbgl
