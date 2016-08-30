#include <mbgl/util/convert.hpp>

namespace mbgl {
namespace util {

template std::array<float, 2> convert(const std::array<int32_t, 2>&);

} // namespace util
} // namespace mbgl
