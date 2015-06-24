#ifndef MBGL_UTIL_IMAGE
#define MBGL_UTIL_IMAGE

#include <string>
#include <memory>

namespace mbgl {
namespace util {

std::string compress_png(int width, int height, const void *rgba);


class Image {
public:
    Image(const std::string &img);

    inline const char *getData() const { return img.get(); }
    inline uint32_t getWidth() const { return width; }
    inline uint32_t getHeight() const { return height; }
    inline operator bool() const { return img && width && height; }

private:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // the raw image data
    std::unique_ptr<char[]> img;
};


}
}

#endif
