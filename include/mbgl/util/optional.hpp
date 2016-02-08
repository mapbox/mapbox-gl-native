#ifndef MBGL_UTIL_OPTIONAL
#define MBGL_UTIL_OPTIONAL

#include <experimental/optional>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

} // namespace mbgl

#endif
