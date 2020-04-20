#include <chrono>

namespace mbgl {
namespace platform {

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> now() {
    return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
}

} // namespace platform
} // namespace mbgl
