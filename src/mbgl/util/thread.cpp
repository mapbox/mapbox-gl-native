#include <functional>
#include <mbgl/platform/settings.hpp>
#include <mbgl/util/platform.hpp>

namespace mbgl {
namespace util {
std::function<void()> makeThreadPrioritySetter(std::string threadType_) {
    return [threadType = std::move(threadType_)] {
        auto& settings = platform::Settings::getInstance();
        auto value = settings.get(threadType);
        if (auto* priority = value.getDouble()) {
            platform::setCurrentThreadPriority(*priority);
        } else {
            platform::makeThreadLowPriority();
        }
    };
}
} // namespace util
} // namespace mbgl
