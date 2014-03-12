#ifndef LLMR_UTIL_RASTER
#define LLMR_UTIL_RASTER

#include <string>
#include <mutex>

namespace llmr {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    ~Raster();

    // load image data
    void load(const std::string& data);

    // bind current texture
    void bind(bool linear = false);

    // loaded status
    operator bool() const;

public:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // uploaded to texture
    bool textured = false;

    // uploaded texture
    uint32_t texture = 0;

private:
    // load raw pixels
    void loadImage(const std::string& data);

private:
    mutable std::mutex mtx;

    // raw pixels loaded
    bool loaded = false;

    // min/mag filter
    uint32_t filter = 0;

    // raw pixels
    char *img = nullptr;
};

}

#endif
