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
#include <unordered_map>
#include <array>
#include <memory>

namespace mbgl {

class FileSource;
class SpriteAtlasObserver;

namespace gl {
class Context;
} // namespace gl

class SpriteAtlasElement {
public:
    SpriteAtlasElement(Rect<uint16_t>, std::shared_ptr<const SpriteImage>, Size size, float pixelRatio);

    Rect<uint16_t> pos;
    std::shared_ptr<const SpriteImage> spriteImage;

    float relativePixelRatio;
    std::array<float, 2> size;
    std::array<float, 2> tl;
    std::array<float, 2> br;
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

    void setSprite(const std::string&, std::shared_ptr<const SpriteImage>);
    void removeSprite(const std::string&);

    std::shared_ptr<const SpriteImage> getSprite(const std::string&);

    optional<SpriteAtlasElement> getIcon(const std::string& name);
    optional<SpriteAtlasElement> getPattern(const std::string& name);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(bool linear, gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    Size getSize() const { return size; }
    float getPixelRatio() const { return pixelRatio; }

    // Only for use in tests.
    void setSprites(const Sprites& sprites);
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

    struct Entry {
        std::shared_ptr<const SpriteImage> spriteImage;

        // One sprite image might be used as both an icon image and a pattern image. If so,
        // it must have two distinct entries in the texture. The one for the icon image has
        // a single pixel transparent border, and the one for the pattern image has a single
        // pixel border wrapped from the opposite side.
        optional<Rect<uint16_t>> iconRect;
        optional<Rect<uint16_t>> patternRect;
    };

    optional<SpriteAtlasElement> getImage(const std::string& name, optional<Rect<uint16_t>> Entry::*rect);
    void copy(const Entry&, optional<Rect<uint16_t>> Entry::*rect);

    std::mutex mutex;
    std::unordered_map<std::string, Entry> entries;
    BinPack<uint16_t> bin;
    PremultipliedImage image;
    mbgl::optional<gl::Texture> texture;
    std::atomic<bool> dirty;
};

} // namespace mbgl
