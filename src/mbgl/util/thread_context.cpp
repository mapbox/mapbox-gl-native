#include <mbgl/util/thread_context.hpp>

namespace mbgl {
namespace util {

class MainThreadContextRegistrar {
public:
    MainThreadContextRegistrar() : context("Main", ThreadType::Main, ThreadPriority::Regular) {
        ThreadContext::current.set(&context);
    }

    ~MainThreadContextRegistrar() {
        ThreadContext::current.set(nullptr);
    }

private:
    ThreadContext context;
};

ThreadContext::ThreadContext(const std::string& name_, ThreadType type_, ThreadPriority priority_)
    : name(name_),
      type(type_),
      priority(priority_) {
}

uv::tls<ThreadContext> ThreadContext::current;

// Will auto register the main thread context
// at startup. Must be instantiated after the
// ThreadContext::current object.
MainThreadContextRegistrar registrar;

}
}
