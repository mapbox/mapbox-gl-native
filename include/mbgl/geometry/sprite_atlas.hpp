#ifndef MBGL_GEOMETRY_SPRITE_ATLAS
#define MBGL_GEOMETRY_SPRITE_ATLAS

#include <mbgl/geometry/binpack.hpp>

#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <set>

namespace mbgl {

class Sprite;
class SpritePosition;

class SpriteAtlas {
public:
    typedef uint16_t dimension;

public:
    // Add way to construct this from another SpriteAtlas (e.g. with another pixelRatio)
    SpriteAtlas(dimension width, dimension height);
    ~SpriteAtlas();

    // Changes the pixel ratio.
    bool resize(float newRatio);

    // Update uninitialized (= outdated) sprites in this atlas from the given sprite.
    void update(const Sprite &sprite);

    // Returns the coordinates of an image that is sourced from the sprite image.
    // This getter attempts to read the image from the sprite if it is already loaded.
    // In that case, it copies it into the sprite atlas and returns the dimensions.
    // Otherwise, it returns a 0/0/0/0 rect.
    Rect<dimension> getImage(const std::string &name, const Sprite &sprite);

    // Returns the coordinates of an image that is sourced from the sprite image.
    // This getter waits until the sprite image was loaded, copies it into the sprite
    // image and returns the dimensions.
    // NEVER CALL THIS FUNCTION FROM THE RENDER THREAD! it is blocking.
    Rect<dimension> waitForImage(const std::string &name, const Sprite &sprite);

    // Binds the image buffer of this sprite atlas to the GPU.
    void bind(bool linear = false);

    // Uploads the image buffer to the GPU if it is out of date.
    void upload();

    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }
    inline float getTextureWidth() const { return width * pixelRatio; }
    inline float getTextureHeight() const { return height * pixelRatio; }
    inline float getPixelRatio() const { return pixelRatio; }

private:
    void allocate();
    Rect<SpriteAtlas::dimension> allocateImage(size_t width, size_t height);
    void copy(const Rect<dimension> &dst, const SpritePosition &src, const Sprite &sprite);

public:
    const dimension width = 0;
    const dimension height = 0;

private:
    std::mutex mtx;
    float pixelRatio = 1.0f;
    BinPack<dimension> bin;
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
