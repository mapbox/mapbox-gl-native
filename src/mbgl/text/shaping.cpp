#include <mbgl/text/shaping.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

PositionedIcon shapeIcon(const SpriteAtlasElement& image, const style::SymbolLayoutProperties& layout) {
    float dx = layout.iconOffset.value[0];
    float dy = layout.iconOffset.value[1];
    float x1 = dx - image.spriteImage->getWidth() / 2.0f;
    float x2 = x1 + image.spriteImage->getWidth();
    float y1 = dy - image.spriteImage->getHeight() / 2.0f;
    float y2 = y1 + image.spriteImage->getHeight();

    return PositionedIcon(image, y1, y2, x1, x2);
}

} // namespace mbgl
