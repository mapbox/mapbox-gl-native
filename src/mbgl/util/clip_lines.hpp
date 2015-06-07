#ifndef MBGL_UTIL_CLIP_LINES
#define MBGL_UTIL_CLIP_LINES

#include <map>
#include <string>
#include <vector>
#include <mbgl/renderer/symbol_bucket.hpp>

namespace mbgl {
namespace util {


std::vector<std::vector<Coordinate>> clipLines(const std::vector<std::vector<Coordinate>> &lines,
        const int16_t x1, const int16_t y1, const int16_t x2, const int16_t y2);

} // end namespace util
} // end namespace mbgl

#endif
