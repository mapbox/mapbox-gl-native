#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

class FillAnnotationImpl : public ShapeAnnotationImpl {
public:
    FillAnnotationImpl(AnnotationID, FillAnnotation);

    void updateStyle(style::Style::Impl&) const final;
    const ShapeAnnotationGeometry& geometry() const final;

private:
    const FillAnnotation annotation;
};

} // namespace mbgl
