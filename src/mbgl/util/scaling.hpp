#ifndef MBGL_UTIL_SCALING
#define MBGL_UTIL_SCALING


#include <mbgl/util/vec.hpp>
#include <mbgl/util/rect.hpp>

#include <cstdint>

namespace mbgl {
namespace util {

void bilinearScale(const uint32_t* srcData, const vec2<uint32_t>& srcSize,
                   const Rect<uint32_t>& srcPos, uint32_t* dstData, const vec2<uint32_t>& dstSize,
                   const Rect<uint32_t>& dstPos, bool wrap);

void nearestNeighborScale(const uint32_t* srcData, const vec2<uint32_t>& srcSize,
                          const Rect<uint32_t>& srcPos, uint32_t* dstData,
                          const vec2<uint32_t>& dstSize, const Rect<uint32_t>& dstPos);
}
}

#endif
