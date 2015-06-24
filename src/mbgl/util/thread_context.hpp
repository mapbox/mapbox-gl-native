#ifndef MBGL_UTIL_THREAD_CONTEXT
#define MBGL_UTIL_THREAD_CONTEXT

#include <mbgl/util/uv_detail.hpp>

#include <cstdint>
#include <string>
#include <thread>

namespace mbgl {

class FileSource;

namespace util {

class GLObjectStore;

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

    static bool currentlyOn(ThreadType type) {
        return current.get()->type == type;
    }

    static std::string getName() {
        return current.get()->name;
    }

    static ThreadPriority getPriority() {
        return current.get()->priority;
    }

    static FileSource* getFileSource() {
        return current.get()->fileSource;
    }

    static void setFileSource(FileSource* fileSource) {
        current.get()->fileSource = fileSource;
    }

    static GLObjectStore* getGLObjectStore() {
        return current.get()->glObjectStore;
    }

    static void setGLObjectStore(GLObjectStore* glObjectStore) {
        current.get()->glObjectStore = glObjectStore;
    }

private:
    std::string name;
    ThreadType type;
    ThreadPriority priority;

    FileSource* fileSource = nullptr;
    GLObjectStore* glObjectStore = nullptr;

    static uv::tls<ThreadContext> current;

    friend class MainThreadContextRegistrar;
    template <class Object> friend class Thread;
};

}
}

#endif
