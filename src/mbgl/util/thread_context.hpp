#ifndef MBGL_UTIL_THREAD_CONTEXT
#define MBGL_UTIL_THREAD_CONTEXT

#include <cstdint>
#include <string>
#include <thread>

namespace mbgl {
namespace util {

enum class ThreadPriority : bool {
    Regular,
    Low,
};

enum class ThreadType : uint8_t {
    Main,
    Map = Main,
    Worker,
    Unknown,
};

struct ThreadContext {
public:
    ThreadContext(const std::string& name, ThreadType type, ThreadPriority priority);

    static void Set(ThreadContext* context);

    static bool currentlyOn(ThreadType type);
    static std::string getName();
    static ThreadPriority getPriority();

    std::string name;
    ThreadType type;
    ThreadPriority priority;
};

} // namespace util
} // namespace mbgl

#endif
