#include <mbgl/util/util.hpp>

#include <array>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

template<typename To, typename From, std::size_t Size,
         typename = std::enable_if_t<std::is_convertible<From, To>::value>>
MBGL_CONSTEXPR std::array<To, Size> convert(const std::array<From, Size>&from) {
    std::array<To, Size> to {{}};
    std::copy(std::begin(from), std::end(from), std::begin(to));
    return to;
}

} // namespace util
} // namespace mbgl
