#ifndef MBGL_MAP_STILL_IMAGE
#define MBGL_MAP_STILL_IMAGE

#include <mbgl/util/noncopyable.hpp>

#include <cstdint>

namespace mbgl {

class StillImage : util::noncopyable {
public:
    size_t width = 0;
    size_t height = 0;
    std::unique_ptr<uint8_t[]> pixels;
};

}

#endif
