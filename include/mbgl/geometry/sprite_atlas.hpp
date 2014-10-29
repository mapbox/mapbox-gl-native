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

class Sprite;
class SpritePosition;

struct SpriteAtlasPosition {
    std::array<float, 2> size;
    std::array<float, 2> tl;
    std::array<float, 2> br;
};

class SpriteAtlas : public util::noncopyable {
public:
    typedef uint16_t dimension;

    // Add way to construct this from another SpriteAtlas (e.g. with another pixelRatio)
    SpriteAtlas(dimension width, dimension height);
    ~SpriteAtlas();

    // Changes the pixel ratio.
    bool resize(float newRatio);

    // Changes the source sprite.
    void setSprite(util::ptr<Sprite> sprite);

    // Returns the coordinates of an image that is sourced from the sprite image.
    // This getter attempts to read the image from the sprite if it is already loaded.
    // In that case, it copies it into the sprite atlas and returns the dimensions.
    // Otherwise, it returns a 0/0/0/0 rect.
    Rect<dimension> getImage(const std::string& name);

    SpriteAtlasPosition getPosition(const std::string& name, bool repeating = false);

    // Binds the image buffer of this sprite atlas to the GPU, and uploads data if it is out
    // of date.
    void bind(bool linear = false);

    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }
    inline float getTextureWidth() const { return width * pixelRatio; }
    inline float getTextureHeight() const { return height * pixelRatio; }
    inline float getPixelRatio() const { return pixelRatio; }

    const dimension width = 0;
    const dimension height = 0;

private:
    void allocate();
    Rect<SpriteAtlas::dimension> allocateImage(size_t width, size_t height);
    void copy(const Rect<dimension>& dst, const SpritePosition& src);

    std::recursive_mutex mtx;
    float pixelRatio = 1.0f;
    BinPack<dimension> bin;
    util::ptr<Sprite> sprite;
    std::map<std::string, Rect<dimension>> images;
    std::set<std::string> uninitialized;
    uint32_t *data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    uint32_t filter = 0;
    static const int buffer = 1;
};

};

#endif
