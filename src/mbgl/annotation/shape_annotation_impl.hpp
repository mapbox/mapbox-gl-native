#pragma once

#include <mbgl/annotation/annotation.hpp>

#include <string>

namespace mbgl {

namespace style {
class Style;
} // namespace style

/**
 * Holds the data needed to update the style for this shape annotation.
 */
class ShapeAnnotationImpl {
public:
    ShapeAnnotationImpl(const AnnotationID, const uint8_t);
    virtual ~ShapeAnnotationImpl() = default;

    virtual void updateStyle(style::Style&) const = 0;

    const AnnotationID id;
    const uint8_t maxZoom;
    const std::string layerID;
};

} // namespace mbgl
