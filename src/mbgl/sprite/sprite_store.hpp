#ifndef MBGL_SPRITE_STORE
#define MBGL_SPRITE_STORE

#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <map>
#include <memory>
#include <mutex>

namespace mbgl {

class SpriteStore : private util::noncopyable {
public:
    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;

    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onSpriteLoaded() = 0;
        virtual void onSpriteLoadingFailed(std::exception_ptr) = 0;
    };

    SpriteStore(float pixelRatio);
    ~SpriteStore();

    void setURL(const std::string&);

    bool isLoaded() const {
        return loaded;
    }

    void dumpDebugLogs() const;

    void setObserver(Observer* observer);

    // Adds/replaces a Sprite image.
    void setSprite(const std::string&, std::shared_ptr<const SpriteImage> = nullptr);

    // Adds/replaces mutliple Sprite images.
    void setSprites(const Sprites& sprites);

    // Removes a Sprite.
    void removeSprite(const std::string&);

    // Obtains a Sprite image.
    std::shared_ptr<const SpriteImage> getSprite(const std::string&);

    // Returns Sprite images that changed since the last invocation of this function.
    Sprites getDirty();

    const float pixelRatio;

private:
    void _setSprite(const std::string&, const std::shared_ptr<const SpriteImage>& = nullptr);

    void emitSpriteLoadedIfComplete();
    void emitSpriteLoadingFailed(const std::string& message);

    struct Loader;
    std::unique_ptr<Loader> loader;

    bool loaded = false;

    Observer* observer = nullptr;

    // Lock for sprites and dirty maps.
    std::mutex mutex;

    // Stores all current sprites.
    Sprites sprites;

    // Stores all Sprite IDs that changed since the last invocation.
    Sprites dirty;
};

} // namespace mbgl

#endif
