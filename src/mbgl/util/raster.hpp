#ifndef MBGL_UTIL_RASTER
#define MBGL_UTIL_RASTER

#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/chrono.hpp>

#include <mutex>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    Raster(TexturePool&);
    ~Raster();

    // load image data
    bool load(std::unique_ptr<util::Image> image);

    // bind current texture
    void bind(bool linear = false);

    // uploads the texture if it hasn't been uploaded yet.
    void upload();

    // loaded status
    bool isLoaded() const;

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
    mutable std::mutex mtx;

    // raw pixels have been loaded
    bool loaded = false;

    // shared texture pool
    TexturePool& texturePool;

    // min/mag filter
    uint32_t filter = 0;

    // the raw pixels
    std::unique_ptr<util::Image> img;
};

}

#endif
