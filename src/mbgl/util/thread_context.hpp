#ifndef MBGL_UTIL_THREAD_CONTEXT
#define MBGL_UTIL_THREAD_CONTEXT

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/uv_detail.hpp>

#include "resource.hpp"

#include <cstdint>
#include <string>
#include <thread>
#include <vector>

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

//    static std::vector<FileSource*> getFileSources() {
//        if (current.get() != nullptr) {
//            return current.get()->fileSources;
//        } else {
//            return std::vector<FileSource*>();
//        }
//    }

    static FileSource* getFileSourceHandling(const Resource& res) {
        if (current.get() != nullptr) {
            for (auto fs : current.get()->fileSources) {
                if (fs->canHandle(res)) {
                    return fs;
                }
            }
        }
        return nullptr;
    }

    static void addFileSource(FileSource* fileSource) {
        if (current.get() != nullptr) {
            current.get()->fileSources.push_back(fileSource);
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

    std::vector<FileSource *> fileSources;
    GLObjectStore* glObjectStore = nullptr;

    static uv::tls<ThreadContext> current;

    friend class MainThreadContextRegistrar;
    template <class Object> friend class Thread;
};

}
}

#endif
