#ifndef MBGL_MAP_STILL_IMAGE
#define MBGL_MAP_STILL_IMAGE

#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <cstdint>

namespace mbgl {

class StillImage : util::noncopyable {
public:
    uint16_t width = 0;
    uint16_t height = 0;
    using Pixel = uint32_t;
    std::unique_ptr<Pixel[]> pixels;
};

}

#endif
