#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/annotation/shape_annotation_impl.hpp>

namespace mbgl {

/**
 * Holds the data needed to update the style for this line annotation.
 */
class LineAnnotationImpl : public ShapeAnnotationImpl {
public:
    LineAnnotationImpl(const AnnotationID&, const LineAnnotation&, const uint8_t maxZoom);

    void updateStyle(style::Style&) const final;

private:
    style::DataDrivenPropertyValue<float> opacity { 1.0f };
    style::PropertyValue<float> width { 1.0f };
    style::DataDrivenPropertyValue<Color> color { Color::black() };
};

} // namespace mbgl
