#include <mbgl/util/thread_context.hpp>

namespace mbgl {
namespace util {

ThreadContext::ThreadContext(const std::string& name_, ThreadPriority priority_)
    : name(name_),
      priority(priority_) {
}

} // namespace util
} // namespace mbgl
