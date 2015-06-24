#ifndef MBGL_ANNOTATION_SPRITE_STORE
#define MBGL_ANNOTATION_SPRITE_STORE

#include <mbgl/annotation/sprite_image.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geo.hpp>

#include <map>
#include <set>
#include <vector>
#include <memory>
#include <mutex>
#include <cstdint>

namespace mbgl {

// The SpriteStore object holds Sprite images.
class SpriteStore : private util::noncopyable {
    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;

public:
    SpriteStore(const float ratio);

    // Adds/replaces a Sprite image.
    void setSprite(const std::string&, std::shared_ptr<const SpriteImage> = nullptr);

    // Removes a Sprite.
    void removeSprite(const std::string&);

    // Obtains a Sprite image.
    std::shared_ptr<const SpriteImage> getSprite(const std::string&);

    // Returns Sprite images that changed since the last invocation of this function.
    Sprites getDirty();

    // Pixel ratio of all sprite images in this storage object.
    const float pixelRatio;

private:
    void _setSprite(const std::string&, const std::shared_ptr<const SpriteImage>& = nullptr);

    // Lock for sprites and dirty maps.
    std::mutex mutex;

    // Stores all current sprites.
    Sprites sprites;

    // Stores all Sprite IDs that changed since the last invocation.
    Sprites dirty;
};

} // namespace mbgl

#endif
