#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

class LineAnnotationImpl : public ShapeAnnotationImpl {
public:
    LineAnnotationImpl(AnnotationID, LineAnnotation);

    void updateStyle(style::Style::Impl&) const final;
    const ShapeAnnotationGeometry& geometry() const final;

private:
    const LineAnnotation annotation;
};

} // namespace mbgl
