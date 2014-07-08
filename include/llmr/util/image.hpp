#ifndef LLMR_UTIL_IMAGE
#define LLMR_UTIL_IMAGE

#include <string>
#include <cstring>
#include <stdexcept>

namespace llmr {
namespace util {

std::string compress_png(int width, int height, void *rgba, bool flip = false);


class Image {
public:
    Image(const std::string &img, bool flip = false);
    ~Image();

    inline const char *getData() const { return img; }
    inline uint32_t getWidth() const { return width; }
    inline uint32_t getHeight() const { return height; }

private:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // the raw image data
    char *img = nullptr;

};


}
}

#endif
