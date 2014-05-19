#ifndef LLMR_GEOMETRY_SPRITE_ATLAS
#define LLMR_GEOMETRY_SPRITE_ATLAS

#include <llmr/geometry/binpack.hpp>

#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <set>

namespace llmr {

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

    // Update uninitialized sprites in this atlas from the given sprite.
    void update(const Sprite &sprite);

    // Returns the coordinates of a square icon. The getter also *creates* new square icons in the
    // atlas if they don't exist, but they'll be default-initialized with a a black circle.
    Rect<dimension> getIcon(int size, const std::string &name);

    // Returns the coordinates of an image that is sourced from the sprite image.
    // This getter does not create images, as the dimension of the texture us unknown if the
    // sprite is not yet loaded. Instead, it returns a 0/0/0/0 rect.
    Rect<dimension> getImage(const std::string &name, const Sprite &sprite);

    // Binds the image buffer of this sprite atlas to the GPU, and uploads data if it is out
    // of date.
    void bind(bool linear = false);

    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }
    inline float getTextureWidth() const { return width * pixelRatio; }
    inline float getTextureHeight() const { return height * pixelRatio; }

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
    char *data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    uint32_t filter = 0;
    static const int buffer = 1;
};

};

#endif
