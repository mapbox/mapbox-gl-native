#ifndef LLMR_UTIL_RASTER
#define LLMR_UTIL_RASTER

#include <llmr/util/animation.hpp>
#include <llmr/util/texturepool.hpp>

#include <string>
#include <mutex>
#include <memory>

typedef struct uv_loop_s uv_loop_t;

namespace llmr {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    ~Raster();

    // load image data
    void load();

    // set shared texture pool
    void setTexturepool(Texturepool* texturepool);

    // bind current texture
    void bind(bool linear = false);

    // loaded status
    bool isLoaded() const;

    // animations
    void beginFadeInAnimation();
    bool needsAnimation() const;
    void updateAnimations(double time);

    inline void setData(const std::string &img) { data = img; }

public:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // has been uploaded to texture
    bool textured = false;

    // the uploaded texture
    uint32_t texture = 0;

    // texture opacity
    double opacity = 0;

private:
    // load raw pixels
    void loadImage(const std::string& data);

private:
    mutable std::mutex mtx;

    // raw pixels have been loaded
    bool loaded = false;

    // shared texture pool
    Texturepool* texturepool = nullptr;

    // min/mag filter
    uint32_t filter = 0;

    // the encoded image data.
    std::string data;

    // the raw pixels
    char *img = nullptr;

    // fade in animation
    std::shared_ptr<util::animation> fade_animation = nullptr;
};

}

#endif
