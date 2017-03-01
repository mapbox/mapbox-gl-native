#include <mbgl/text/shaping.hpp>
#include <mbgl/layout/symbol_feature.hpp>

namespace mbgl {

PositionedIcon shapeIcon(const SpriteAtlasElement& image, const std::array<float, 2>& iconOffset, const float iconRotation) {
    float dx = iconOffset[0];
    float dy = iconOffset[1];
    float x1 = dx - image.spriteImage->getWidth() / 2.0f;
    float x2 = x1 + image.spriteImage->getWidth();
    float y1 = dy - image.spriteImage->getHeight() / 2.0f;
    float y2 = y1 + image.spriteImage->getHeight();

    return PositionedIcon(image, y1, y2, x1, x2, iconRotation);
}

} // namespace mbgl
