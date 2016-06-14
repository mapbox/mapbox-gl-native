#pragma once

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/atomic.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <map>
#include <mutex>
#include <set>
#include <array>

namespace mbgl {

class SpriteStore;
class SpriteImage;
class SpritePosition;

struct SpriteAtlasPosition {
    std::array<float, 2> size = {{ 0, 0 }};
    std::array<float, 2> tl = {{ 0, 0 }};
    std::array<float, 2> br = {{ 0, 0 }};
};

struct SpriteAtlasElement {
    Rect<uint16_t> pos;
    std::shared_ptr<const SpriteImage> spriteImage;
    float relativePixelRatio;
};

class SpriteAtlas : public util::noncopyable {
public:
    typedef uint16_t dimension;

    SpriteAtlas(dimension width, dimension height, float pixelRatio, SpriteStore& store);
    ~SpriteAtlas();

    // If the sprite is loaded, copies the requsted image from it into the atlas and returns
    // the resulting icon measurements. If not, returns an empty optional.
    optional<SpriteAtlasElement> getImage(const std::string& name, const bool wrap);

    // This function is used for getting the position during render time.
    optional<SpriteAtlasPosition> getPosition(const std::string& name, bool repeating = false);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(bool linear, gl::ObjectStore&);

    // Updates sprites in the atlas texture that may have changed in the source SpriteStore object.
    void updateDirty();

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::ObjectStore&);

    inline dimension getWidth() const { return width; }
    inline dimension getHeight() const { return height; }
    inline dimension getTextureWidth() const { return pixelWidth; }
    inline dimension getTextureHeight() const { return pixelHeight; }
    inline float getPixelRatio() const { return pixelRatio; }

    // Only for use in tests.
    inline const uint32_t* getData() const { return data.get(); }

private:
    const GLsizei width, height;
    const dimension pixelWidth, pixelHeight;
    const float pixelRatio;

    struct Holder : private util::noncopyable {
        inline Holder(std::shared_ptr<const SpriteImage>, Rect<dimension>);
        inline Holder(Holder&&);
        std::shared_ptr<const SpriteImage> spriteImage;
        const Rect<dimension> pos;
    };

    using Key = std::pair<std::string, bool>;

    Rect<SpriteAtlas::dimension> allocateImage(const SpriteImage&);
    void copy(const Holder& holder, const bool wrap);

    std::recursive_mutex mtx;
    SpriteStore& store;
    BinPack<dimension> bin;
    std::map<Key, Holder> images;
    std::set<std::string> uninitialized;
    std::unique_ptr<uint32_t[]> data;
    util::Atomic<bool> dirty;
    bool fullUploadRequired = true;
    mbgl::optional<gl::UniqueTexture> texture;
    uint32_t filter = 0;
    static const int buffer = 1;
};

} // namespace mbgl
