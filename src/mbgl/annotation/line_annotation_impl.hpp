#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

class LineAnnotationImpl : public ShapeAnnotationImpl {
public:
    LineAnnotationImpl(const AnnotationID, const LineAnnotation&, const uint8_t maxZoom);

    void updateStyle(Style&) const final;
    const Geometry<double>& geometry() const final;

private:
    const LineAnnotation annotation;
};

} // namespace mbgl
