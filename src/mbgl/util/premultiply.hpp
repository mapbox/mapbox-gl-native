#ifndef MBGL_UTIL_PREMULTIPLY
#define MBGL_UTIL_PREMULTIPLY

#include <mbgl/util/image.hpp>

namespace mbgl {
namespace util {

PremultipliedImage premultiply(UnassociatedImage&&);
UnassociatedImage unpremultiply(PremultipliedImage&&);

}
}

#endif
