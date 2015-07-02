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
    struct Data {
        std::string image;
        std::string json;
    };

    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onSpriteDataLoaded(std::unique_ptr<Data>) = 0;
        virtual void onSpriteLoaded() = 0;
        virtual void onSpriteLoadingFailed(std::exception_ptr error) = 0;
    };

    Sprite(const std::string& baseUrl, float pixelRatio);
    ~Sprite();

    inline bool isLoaded() const {
        return loader == nullptr;
    }

    const float pixelRatio;

    void setObserver(Observer* observer);

private:
    void emitSpriteLoadedIfComplete();
    void emitSpriteLoadingFailed(const std::string& message);

    struct Loader;
    std::unique_ptr<Loader> loader;

    Observer* observer = nullptr;
};

} // namespace mbgl

#endif
