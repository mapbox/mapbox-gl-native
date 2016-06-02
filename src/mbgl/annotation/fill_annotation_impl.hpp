#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

class FillAnnotationImpl : public ShapeAnnotationImpl {
public:
    FillAnnotationImpl(const AnnotationID, const FillAnnotation&, const uint8_t maxZoom);

    void updateStyle(Style&) const final;
    const ShapeAnnotationGeometry& geometry() const final;

private:
    const FillAnnotation annotation;
};

} // namespace mbgl
