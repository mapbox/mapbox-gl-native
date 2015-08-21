#ifndef MBGL_GEOMETRY_SPRITE_ATLAS
#define MBGL_GEOMETRY_SPRITE_ATLAS

#include <mbgl/geometry/binpack.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/ptr.hpp>

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
    const Rect<uint16_t> pos;
    const std::shared_ptr<const SpriteImage> texture;
};

class SpriteAtlas : public util::noncopyable {
public:
    typedef uint16_t dimension;

    SpriteAtlas(dimension width, dimension height, float pixelRatio, SpriteStore& store);
    ~SpriteAtlas();

    // Returns the coordinates of an image that is sourced from the sprite image.
    // This getter attempts to read the image from the sprite if it is already loaded.
    // In that case, it copies it into the sprite atlas and returns the dimensions.
    // Otherwise, it returns a 0/0/0/0 rect.
    // This function is used during bucket creation.
    SpriteAtlasElement getImage(const std::string& name, const bool wrap);

    // This function is used for getting the position during render time.
    SpriteAtlasPosition getPosition(const std::string& name, bool repeating = false);

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
    inline const uint32_t* getData() const { return data.get(); }

private:
    const dimension width, height;
    const dimension pixelWidth, pixelHeight;
    const float pixelRatio;

    struct Holder : private util::noncopyable {
        inline Holder(const std::shared_ptr<const SpriteImage>&, const Rect<dimension>&);
        inline Holder(Holder&&);
        std::shared_ptr<const SpriteImage> texture;
        const Rect<dimension> pos;
    };

    using Key = std::pair<std::string, bool>;

    Rect<SpriteAtlas::dimension> allocateImage(size_t width, size_t height);
    void copy(const Holder& holder, const bool wrap);

    std::recursive_mutex mtx;
    SpriteStore& store;
    BinPack<dimension> bin;
    std::map<Key, Holder> images;
    std::set<std::string> uninitialized;
    const std::unique_ptr<uint32_t[]> data;
    std::atomic<bool> dirty;
    bool fullUploadRequired = true;
    uint32_t texture = 0;
    uint32_t filter = 0;
    static const int buffer = 1;
};

};

#endif
