#ifndef LLMR_UTIL_RASTER
#define LLMR_UTIL_RASTER

#include <llmr/util/transition.hpp>
#include <llmr/util/texturepool.hpp>

#include <string>
#include <mutex>
#include <memory>

typedef struct uv_loop_s uv_loop_t;

namespace llmr {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    Raster(Texturepool &texturepool);
    ~Raster();

    // load image data
    bool load(const std::string &img);

    // bind current texture
    void bind(bool linear = false);

    // loaded status
    bool isLoaded() const;

    // transitions
    void beginFadeInTransition();
    bool needsTransition() const;
    void updateTransitions(time now);

    inline const char *getData() const { return img; }

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
    Texturepool &texturepool;

    // min/mag filter
    uint32_t filter = 0;

    // the raw pixels
    char *img = nullptr;

    // fade in transition
    std::shared_ptr<util::transition> fade_transition = nullptr;
};

}

#endif
