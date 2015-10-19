#include <mbgl/text/shaping.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

PositionedIcon shapeIcon(const Rect<uint16_t>& image, const SymbolLayoutProperties& layout) {
    float dx = layout.icon.offset[0];
    float dy = layout.icon.offset[1];
    float x1 = dx - image.originalW / 2.0f;
    float x2 = x1 + image.originalW;
    float y1 = dy - image.originalH / 2.0f;
    float y2 = y1 + image.originalH;

    return PositionedIcon(image, y1, y2, x1, x2);
}

}
