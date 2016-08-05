#include <mbgl/util/platform.hpp>

#include <string>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    return "unknown";
}

void setCurrentThreadName(const std::string&) {
}

void makeThreadLowPriority() {
}

} // namespace platform
} // namespace mbgl
