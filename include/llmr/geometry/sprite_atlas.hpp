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

    // Returns the coordinates of the icon. The getter also *creates* new icons in the atlas
    // if they don't exist, but they'll be default-initialized with a a black circle.
    Rect<dimension> getIcon(int size, const std::string &name);

    // Updates or creates an icon with a given size and name. The data must be a
    // a (pixelRatio * size)^2 * 4 byte long RGBA image buffer.
    Rect<dimension> setIcon(int size, const std::string &name, const std::string &icon);

    // Binds the image buffer of this sprite atlas to the GPU, and uploads data if it is out
    // of date.
    void bind(bool linear = false);

    inline float getTextureWidth() const { return width * pixelRatio; }
    inline float getTextureHeight() const { return height * pixelRatio; }

private:
    void allocate();

public:
    const dimension width = 0;
    const dimension height = 0;

private:
    std::mutex mtx;
    float pixelRatio = 1.0f;
    BinPack<dimension> bin;
    std::map<int, std::map<std::string, Rect<dimension>>> index;
    std::set<std::pair<int, std::string>> uninitialized;
    char *data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
    uint32_t filter = 0;
    static const int buffer = 1;
};

};

#endif
