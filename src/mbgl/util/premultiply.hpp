#pragma once

#include <cstdint>
#include <cstddef>

namespace mbgl {
namespace util {

// (Un-)Premultiples a blob of RGBA image data with the given length.
void premultiply(uint8_t* data, size_t size);
void unpremultiply(uint8_t* data, size_t size);

} // namespace util
} // namespace mbgl
