#ifndef MBGL_STYLE_SPRITE
#define MBGL_STYLE_SPRITE

#include <mbgl/util/image.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/storage/request.hpp>

#include <cstdint>
#include <atomic>
#include <iosfwd>
#include <string>
#include <unordered_map>

namespace mbgl {

class Request;

class SpritePosition {
public:
    explicit SpritePosition() {}
    explicit SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, float pixelRatio, bool sdf);

    operator bool() const {
        return !(width == 0 && height == 0 && x == 0 && y == 0);
    }

    uint16_t x = 0, y = 0;
    uint16_t width = 0, height = 0;
    float pixelRatio = 1.0f;
    bool sdf = false;
};

class Sprite : private util::noncopyable {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onSpriteLoaded() = 0;
        virtual void onSpriteLoadingFailed(std::exception_ptr error) = 0;
    };

    Sprite(const std::string& baseUrl, float pixelRatio);
    ~Sprite();

    const SpritePosition &getSpritePosition(const std::string& name) const;

    bool hasPixelRatio(float ratio) const;

    bool isLoaded() const;

    const float pixelRatio;
    std::unique_ptr<util::Image> raster;

    void setObserver(Observer* observer);
private:
    void emitSpriteLoadedIfComplete();
    void emitSpriteLoadingFailed(const std::string& message);

    void parseJSON(const std::string& jsonURL);
    void parseImage(const std::string& spriteURL);

    std::string body;
    std::string image;
    std::atomic<bool> loadedImage;
    std::atomic<bool> loadedJSON;
    std::unordered_map<std::string, SpritePosition> pos;
    const SpritePosition empty;

    Request* jsonRequest = nullptr;
    Request* spriteRequest = nullptr;
    Observer* observer = nullptr;
};

}

#endif
