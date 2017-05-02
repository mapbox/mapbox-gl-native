#pragma once

#include <mbgl/util/image.hpp>

namespace mbgl {
namespace util {

PremultipliedImage premultiply(UnassociatedImage&&);
UnassociatedImage unpremultiply(PremultipliedImage&&);

} // namespace util
} // namespace mbgl
