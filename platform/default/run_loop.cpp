#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/uv.hpp>

namespace mbgl {
namespace util {

static uv::tls<RunLoop> current;

RunLoop* RunLoop::Get() {
    return current.get();
}

class RunLoop::Impl {
public:
    Impl() = default;

    uv_loop_t *loop;
    RunLoop::Type type;
    std::unique_ptr<AsyncTask> async;
};

RunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>()) {
    switch (type) {
    case Type::New:
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
        impl->loop = uv_loop_new();
        if (impl->loop == nullptr) {
#else
        impl->loop = new uv_loop_t;
        if (uv_loop_init(impl->loop) != 0) {
#endif
            throw std::runtime_error("Failed to initialize loop.");
        }
        break;
    case Type::Default:
        impl->loop = uv_default_loop();
        break;
    }

    impl->type = type;

    current.set(this);
    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    current.set(nullptr);

    if (impl->type == Type::Default) {
        return;
    }

    // Run the loop again to ensure that async
    // close callbacks have been called. Not needed
    // for the default main loop because it is only
    // closed when the application exits.
    impl->async.reset();
    runOnce();

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    uv_loop_delete(impl->loop);
#else
    if (uv_loop_close(impl->loop) == UV_EBUSY) {
        throw std::runtime_error("Failed to close loop.");
    }
    delete impl->loop;
#endif
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    return current.get()->impl->loop;
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(task); });
    impl->async->send();
}

void RunLoop::run() {
    uv_run(impl->loop, UV_RUN_DEFAULT);
}

void RunLoop::runOnce() {
    uv_run(impl->loop, UV_RUN_ONCE);
}

void RunLoop::stop() {
    invoke([&] { impl->async->unref(); });
}

}
}
