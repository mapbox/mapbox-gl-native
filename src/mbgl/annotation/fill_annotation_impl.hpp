#pragma once

#include <mbgl/annotation/shape_annotation_impl.hpp>
#include <mbgl/annotation/annotation.hpp>

namespace mbgl {

/**
 * Holds the data needed to update the style for this fill annotation.
 */
class FillAnnotationImpl : public ShapeAnnotationImpl {
public:
    FillAnnotationImpl(const AnnotationID&, const FillAnnotation&, const uint8_t maxZoom);

    void updateStyle(style::Style&) const final;

private:
    style::DataDrivenPropertyValue<float> opacity { 1.0f };
    style::DataDrivenPropertyValue<Color> color { Color::black() };
    style::DataDrivenPropertyValue<Color> outlineColor {};
};

} // namespace mbgl
