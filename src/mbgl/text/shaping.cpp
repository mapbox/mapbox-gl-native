#include <mbgl/text/shaping.hpp>
#include <mbgl/layout/symbol_feature.hpp>

namespace mbgl {

PositionedIcon shapeIcon(const SpriteAtlasElement& image,
                         const SymbolFeature& feature) {
    float dx = feature.iconOffset[0];
    float dy = feature.iconOffset[1];
    float x1 = dx - image.spriteImage->getWidth() / 2.0f;
    float x2 = x1 + image.spriteImage->getWidth();
    float y1 = dy - image.spriteImage->getHeight() / 2.0f;
    float y2 = y1 + image.spriteImage->getHeight();

    return PositionedIcon(image, y1, y2, x1, x2, feature.iconRotation);
}

} // namespace mbgl
