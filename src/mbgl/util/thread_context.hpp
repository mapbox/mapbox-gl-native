#ifndef MBGL_UTIL_THREAD_CONTEXT
#define MBGL_UTIL_THREAD_CONTEXT

#include <cstdint>
#include <string>
#include <thread>

namespace mbgl {

namespace gl { class GLObjectStore; }

namespace util {

enum class ThreadPriority : bool {
    Regular,
    Low,
};

enum class ThreadType : uint8_t {
    Main,
    Map,
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

    static gl::GLObjectStore* getGLObjectStore();
    static void setGLObjectStore(gl::GLObjectStore* glObjectStore);

    std::string name;
    ThreadType type;
    ThreadPriority priority;

    gl::GLObjectStore* glObjectStore = nullptr;
};

} // namespace util
} // namespace mbgl

#endif
