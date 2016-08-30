#include <mbgl/util/thread_context.hpp>
#include <utility>

namespace mbgl {
namespace util {

ThreadContext::ThreadContext(std::string name_, ThreadPriority priority_)
    : name(std::move(name_)),
      priority(priority_) {
}

} // namespace util
} // namespace mbgl
