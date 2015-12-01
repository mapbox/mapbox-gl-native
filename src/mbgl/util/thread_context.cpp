#include <mbgl/util/thread_context.hpp>

#include <mbgl/util/thread_local.hpp>

#include <cassert>

namespace {

using namespace mbgl::util;
static ThreadLocal<ThreadContext>& current = *new ThreadLocal<ThreadContext>;

} // namespace

namespace mbgl {
namespace util {

ThreadContext::ThreadContext(const std::string& name_, ThreadType type_, ThreadPriority priority_)
    : name(name_),
      type(type_),
      priority(priority_) {
}

void ThreadContext::Set(ThreadContext* context) {
    current.set(context);
}

bool ThreadContext::currentlyOn(ThreadType type) {
    return current.get()->type == type;
}

std::string ThreadContext::getName() {
    if (current.get() != nullptr) {
        return current.get()->name;
    } else {
        return "Unknown";
    }
}

ThreadPriority ThreadContext::getPriority() {
    if (current.get() != nullptr) {
        return current.get()->priority;
    } else {
        return ThreadPriority::Regular;
    }
}

FileSource* ThreadContext::getFileSource() {
    if (current.get() != nullptr) {
        return current.get()->fileSource;
    } else {
        return nullptr;
    }
}

void ThreadContext::setFileSource(FileSource* fileSource) {
    if (current.get() != nullptr) {
        current.get()->fileSource = fileSource;
    } else {
        throw std::runtime_error("Current thread has no current ThreadContext.");
    }
}

GLObjectStore* ThreadContext::getGLObjectStore() {
    if (current.get() != nullptr) {
        return current.get()->glObjectStore;
    } else {
        return nullptr;
    }
}

void ThreadContext::setGLObjectStore(GLObjectStore* glObjectStore) {
    if (current.get() != nullptr) {
        current.get()->glObjectStore = glObjectStore;
    } else {
        throw std::runtime_error("Current thread has no current ThreadContext.");
    }
}

class MainThreadContextRegistrar {
public:
    MainThreadContextRegistrar() : context("Main", ThreadType::Main, ThreadPriority::Regular) {
        ThreadContext::Set(&context);
    }

    ~MainThreadContextRegistrar() {
        ThreadContext::Set(nullptr);
    }

private:
    ThreadContext context;
};

// Will auto register the main thread context
// at startup. Must be instantiated after the
// ThreadContext::current object.
MainThreadContextRegistrar registrar;

} // namespace util
} // namespace mbgl
