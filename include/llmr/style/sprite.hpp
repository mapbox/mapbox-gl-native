#ifndef LLMR_STYLE_SPRITE
#define LLMR_STYLE_SPRITE

#include <cstdint>
#include <map>

namespace llmr {

class Sprite {
public:
    Sprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio = 1);

    uint16_t x = 0, y = 0;
    uint16_t width = 0, height = 0;
    uint8_t pixelRatio = 1;
};

typedef std::map<std::string, Sprite> Texture;

Texture createTexture(const std::string& data);

}

#endif
