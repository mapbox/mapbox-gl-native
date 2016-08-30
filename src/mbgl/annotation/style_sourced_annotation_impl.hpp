#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

class StyleSourcedAnnotationImpl : public ShapeAnnotationImpl {
public:
    StyleSourcedAnnotationImpl(AnnotationID, StyleSourcedAnnotation, uint8_t maxZoom);

    void updateStyle(style::Style&) const final;
    const ShapeAnnotationGeometry& geometry() const final;

private:
    const StyleSourcedAnnotation annotation;
};

} // namespace mbgl
