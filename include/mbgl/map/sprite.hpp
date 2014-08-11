#ifndef MBGL_STYLE_SPRITE
#define MBGL_STYLE_SPRITE

#include <mbgl/util/image.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <atomic>
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <future>

namespace mbgl {

class FileSource;

class SpritePosition {
public:
    explicit SpritePosition() {}
    explicit SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio = 1);

    operator bool() const {
        return !(width == 0 && height == 0 && x == 0 && y == 0);
    }

    uint16_t x = 0, y = 0;
    uint16_t width = 0, height = 0;
    uint8_t pixelRatio = 1;
};

class Sprite : public std::enable_shared_from_this<Sprite>, private util::noncopyable {
private:
    struct Key {};
    void load(const std::shared_ptr<FileSource> &fileSource);

public:
    Sprite(const Key &, const std::string& base_url, float pixelRatio);
    static std::shared_ptr<Sprite> Create(const std::string& base_url, float pixelRatio, const std::shared_ptr<FileSource> &fileSource);

    const SpritePosition &getSpritePosition(const std::string& name) const;

    void waitUntilLoaded() const;
    bool isLoaded() const;

    operator bool() const;

private:
    const bool valid;

public:
    const float pixelRatio;
    const std::string spriteURL;
    const std::string jsonURL;
    std::unique_ptr<util::Image> raster;

private:
    void parseJSON();
    void parseImage();
    void complete();

private:
    std::string body;
    std::string image;
    std::atomic<bool> loadedImage;
    std::atomic<bool> loadedJSON;
    std::unordered_map<std::string, SpritePosition> pos;
    const SpritePosition empty;

    std::promise<void> promise;
    std::future<void> future;

};

}

#endif
