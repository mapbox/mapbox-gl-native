#ifndef MBGL_ANNOTATION
#define MBGL_ANNOTATION

#include <cstdint>
#include <vector>

namespace mbgl {

enum class AnnotationType : uint8_t {
    Any   = 0,
    Point = 1 << 0,
    Shape = 1 << 1,
};

using AnnotationID = uint32_t;
using AnnotationIDs = std::vector<AnnotationID>;

}

#endif
