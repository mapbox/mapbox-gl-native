#ifndef MBGL_UTIL_IMAGE
#define MBGL_UTIL_IMAGE

#include <string>
#include <memory>

namespace mbgl {
namespace util {

std::string compress_png(size_t width, size_t height, const uint8_t* rgba);


class Image {
public:
    explicit Image(const std::string& img);

    inline const uint8_t* getData() const { return img.get(); }
    inline uint32_t getWidth() const { return width; }
    inline uint32_t getHeight() const { return height; }
    inline operator bool() const { return img && width && height; }

private:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // the raw image data
    std::unique_ptr<uint8_t[]> img;
};


}
}

#endif
