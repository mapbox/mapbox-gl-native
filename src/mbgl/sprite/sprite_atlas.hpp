#ifndef MBGL_SPRITE_ATLAS
#define MBGL_SPRITE_ATLAS

#include <mbgl/geometry/binpack.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/optional.hpp>

#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <set>
#include <array>

namespace mbgl {

class SpriteStore;
class SpriteImage;
class SpritePosition;

struct SpriteAtlasPosition {
    inline SpriteAtlasPosition(const std::array<float, 2> size_ = {{0, 0}},
                               const std::array<float, 2> tl_ = {{0, 0}},
                               const std::array<float, 2> br_ = {{0, 0}})
        : size(size_), tl(tl_), br(br_) {}
    std::array<float, 2> size;
    std::array<float, 2> tl;
    std::array<float, 2> br;
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
    void bind(bool linear = false);

    // Updates sprites in the atlas texture that may have changed in the source SpriteStore object.
    void updateDirty();

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload();

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
        inline Holder(const std::shared_ptr<const SpriteImage>&, const Rect<dimension>&);
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
    std::atomic<bool> dirty;
    bool fullUploadRequired = true;
    GLuint texture = 0;
    uint32_t filter = 0;
    static const int buffer = 1;
};

} // namespace mbgl

#endif
