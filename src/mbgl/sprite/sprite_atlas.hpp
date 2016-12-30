#pragma once

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/gl/texture.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/sprite/sprite_image.hpp>

#include <atomic>
#include <string>
#include <map>
#include <mutex>
#include <unordered_set>
#include <array>
#include <memory>

namespace mbgl {

class FileSource;
class SpriteAtlasObserver;

namespace gl {
class Context;
} // namespace gl

class SpriteImage;
class SpritePosition;

class SpriteAtlasPosition {
public:
    SpriteAtlasPosition(std::array<float, 2> size_, std::array<float, 2> tl_, std::array<float, 2> br_) : size(std::move(size_)), tl(std::move(tl_)), br(std::move(br_)) {}
    std::array<float, 2> size = {{ 0, 0 }};
    std::array<float, 2> tl = {{ 0, 0 }};
    std::array<float, 2> br = {{ 0, 0 }};
};

class SpriteAtlasElement {
public:
    Rect<uint16_t> pos;
    std::shared_ptr<const SpriteImage> spriteImage;
    float relativePixelRatio;
};

enum class SpritePatternMode : bool {
    Single = false,
    Repeating = true,
};

class SpriteAtlas : public util::noncopyable {
public:
    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;

    SpriteAtlas(Size, float pixelRatio);
    ~SpriteAtlas();

    void load(const std::string& url, FileSource&);

    bool isLoaded() const {
        return loaded;
    }

    void dumpDebugLogs() const;

    void setObserver(SpriteAtlasObserver*);

    // Adds/replaces a Sprite image.
    void setSprite(const std::string&, std::shared_ptr<const SpriteImage>);

    // Adds/replaces mutliple Sprite images.
    void setSprites(const Sprites& sprites);

    // Removes a Sprite.
    void removeSprite(const std::string&);

    // Obtains a Sprite image.
    std::shared_ptr<const SpriteImage> getSprite(const std::string&);

    // If the sprite is loaded, copies the requsted image from it into the atlas and returns
    // the resulting icon measurements. If not, returns an empty optional.
    optional<SpriteAtlasElement> getImage(const std::string& name, SpritePatternMode mode);

    // This function is used for getting the position during render time.
    optional<SpriteAtlasPosition> getPosition(const std::string& name,
                                              SpritePatternMode mode = SpritePatternMode::Single);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(bool linear, gl::Context&, gl::TextureUnit unit);

    // Updates sprites in the atlas texture that may have changed.
    void updateDirty();

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const { return size; }
    float getPixelRatio() const { return pixelRatio; }

    // Only for use in tests.
    const PremultipliedImage& getAtlasImage() const {
        return image;
    }

private:
    void _setSprite(const std::string&, const std::shared_ptr<const SpriteImage>& = nullptr);
    void emitSpriteLoadedIfComplete();

    const Size size;
    const float pixelRatio;

    struct Loader;
    std::unique_ptr<Loader> loader;

    bool loaded = false;

    SpriteAtlasObserver* observer = nullptr;

    // Lock for sprites and dirty maps.
    std::mutex mutex;

    // Stores all current sprites.
    Sprites sprites;

    // Stores all Sprite IDs that changed since the last invocation.
    Sprites dirtySprites;

    struct Holder : private util::noncopyable {
        Holder(std::shared_ptr<const SpriteImage>, Rect<uint16_t>);
        Holder(Holder&&);
        std::shared_ptr<const SpriteImage> spriteImage;
        const Rect<uint16_t> pos;
    };

    using Key = std::pair<std::string, SpritePatternMode>;

    Rect<uint16_t> allocateImage(const SpriteImage&);
    void copy(const Holder& holder, SpritePatternMode mode);

    std::recursive_mutex mtx;
    BinPack<uint16_t> bin;
    std::map<Key, Holder> images;
    std::unordered_set<std::string> uninitialized;
    PremultipliedImage image;
    mbgl::optional<gl::Texture> texture;
    std::atomic<bool> dirty;
    static const int buffer = 1;
};

} // namespace mbgl
