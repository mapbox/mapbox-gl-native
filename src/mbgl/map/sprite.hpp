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

    bool isLoaded() const;

    const float pixelRatio;

    void setObserver(Observer* observer);

    inline const std::string& getImage() const { return image; }
    inline const std::string& getJSON() const { return json; }

private:
    void emitSpriteLoadedIfComplete();
    void emitSpriteLoadingFailed(const std::string& message);

    bool loadedJSON = false;
    std::string json;

    bool loadedImage = false;
    std::string image;

    Request* jsonRequest = nullptr;
    Request* spriteRequest = nullptr;
    Observer* observer = nullptr;
};

}

#endif
