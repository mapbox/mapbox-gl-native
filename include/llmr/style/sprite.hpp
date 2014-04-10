#ifndef LLMR_STYLE_SPRITE
#define LLMR_STYLE_SPRITE

#include <map>
#include <string>
#include <mutex>
#include <memory>

#include <llmr/util/raster.hpp>
#include <llmr/util/vec.hpp>

namespace llmr {

class SpritePosition {
public:
    SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio = 1);

    uint16_t x = 0, y = 0;
    uint16_t width = 0, height = 0;
    uint8_t pixelRatio = 1;
};

class ImagePosition {
public:
    ImagePosition() {}
    ImagePosition(const vec2<uint16_t>& size, vec2<float> tl, vec2<float> br);

    vec2<uint16_t> size = { 0, 0 };
    vec2<float> tl = { 0, 0 };
    vec2<float> br = { 0, 0 };
};


class Sprite : public std::enable_shared_from_this<Sprite> {
public:
    Sprite(float pixelRatio = 1);

    void load(const std::string& base_url);

    ImagePosition getPosition(const std::string& name, bool repeating = false);

    operator bool() const;

public:
    const float pixelRatio;
    std::shared_ptr<Raster> raster;

private:
    void parseJSON(const std::string& data);

private:
    mutable std::mutex mtx;
    bool loaded = false;
    std::map<std::string, SpritePosition> pos;
};

}

#endif
