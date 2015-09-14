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
        if (current.get() != nullptr) {
            return current.get()->name;
        } else {
            return "Unknown";
        }
    }

    static ThreadPriority getPriority() {
        if (current.get() != nullptr) {
            return current.get()->priority;
        } else {
            return ThreadPriority::Regular;
        }
    }

    static FileSource* getFileSource() {
        if (current.get() != nullptr) {
            return current.get()->fileSource;
        } else {
            return nullptr;
        }
    }

    static void setFileSource(FileSource* fileSource) {
        if (current.get() != nullptr) {
            current.get()->fileSource = fileSource;
        } else {
            throw new std::runtime_error("Current thread has no current ThreadContext.");
        }
    }

    static GLObjectStore* getGLObjectStore() {
        if (current.get() != nullptr) {
            return current.get()->glObjectStore;
        } else {
            return nullptr;
        }
    }

    static void setGLObjectStore(GLObjectStore* glObjectStore) {
        if (current.get() != nullptr) {
            current.get()->glObjectStore = glObjectStore;
        } else {
            throw new std::runtime_error("Current thread has no current ThreadContext.");
        }
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
