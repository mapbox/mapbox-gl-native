#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/thread_local.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <uv.h>

#include <cassert>
#include <functional>
#include <unordered_map>

namespace {

void dummyCallback(uv_async_t*) {}

} // namespace

namespace mbgl {
namespace util {

struct Watch {
    static void onEvent(uv_poll_t* poll, int, int event) {
        auto watch = reinterpret_cast<Watch*>(poll->data);

        RunLoop::Event watchEvent = RunLoop::Event::None;
        switch (event) {
        case UV_READABLE:
            watchEvent = RunLoop::Event::Read;
            break;
        case UV_WRITABLE:
            watchEvent = RunLoop::Event::Write;
            break;
        case UV_READABLE | UV_WRITABLE:
            watchEvent = RunLoop::Event::ReadWrite;
            break;
        }

        watch->eventCallback(watch->fd, watchEvent);
    };

    static void onClose(uv_handle_t *poll) {
        auto watch = reinterpret_cast<Watch*>(poll->data);
        watch->closeCallback();
    };

    uv_poll_t poll;
    int fd;

    std::function<void(int, RunLoop::Event)> eventCallback;
    std::function<void()> closeCallback;
};

RunLoop* RunLoop::Get() {
    assert(static_cast<RunLoop*>(Scheduler::GetCurrent()));
    return static_cast<RunLoop*>(Scheduler::GetCurrent());
}

class RunLoop::Impl {
public:
    void closeHolder() {
        uv_close(holderHandle(), [](uv_handle_t* h) {
            delete reinterpret_cast<uv_async_t*>(h);
        });
    }

    uv_handle_t* holderHandle() {
        return reinterpret_cast<uv_handle_t*>(holder);
    }

    uv_loop_t *loop = nullptr;
    uv_async_t* holder = new uv_async_t;

    RunLoop::Type type;
    std::unique_ptr<AsyncTask> async;

    std::unordered_map<int, std::unique_ptr<Watch>> watchPoll;
};

RunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>()) {
    switch (type) {
    case Type::New:
        impl->loop = new uv_loop_t;
        if (uv_loop_init(impl->loop) != 0) {
            throw std::runtime_error("Failed to initialize loop.");
        }
        break;
    case Type::Default:
        impl->loop = uv_default_loop();
        break;
    }

    // Just for holding a ref to the main loop and keep
    // it alive as required by libuv.
    if (uv_async_init(impl->loop, impl->holder, dummyCallback) != 0) {
        throw std::runtime_error("Failed to initialize async.");
    }

    impl->type = type;

    Scheduler::SetCurrent(this);
    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    Scheduler::SetCurrent(nullptr);

    // Close the dummy handle that we have
    // just to keep the main loop alive.
    impl->closeHolder();

    if (impl->type == Type::Default) {
        return;
    }

    // Run the loop again to ensure that async
    // close callbacks have been called. Not needed
    // for the default main loop because it is only
    // closed when the application exits.
    impl->async.reset();
    runOnce();

    if (uv_loop_close(impl->loop) == UV_EBUSY) {
        assert(false && "Failed to close loop.");
    }
    delete impl->loop;
}

LOOP_HANDLE RunLoop::getLoopHandle() {
    return Get()->impl->loop;
}

void RunLoop::wake() {
    impl->async->send();
}

void RunLoop::run() {
    MBGL_VERIFY_THREAD(tid);

    uv_ref(impl->holderHandle());
    uv_run(impl->loop, UV_RUN_DEFAULT);
}

void RunLoop::runOnce() {
    MBGL_VERIFY_THREAD(tid);

    uv_run(impl->loop, UV_RUN_NOWAIT);
}

void RunLoop::stop() {
    invoke([&] { uv_unref(impl->holderHandle()); });
}

void RunLoop::addWatch(int fd, Event event, std::function<void(int, Event)>&& callback) {
    MBGL_VERIFY_THREAD(tid);

    Watch *watch = nullptr;
    auto watchPollIter = impl->watchPoll.find(fd);

    if (watchPollIter == impl->watchPoll.end()) {
        std::unique_ptr<Watch> watchPtr = std::make_unique<Watch>();

        watch = watchPtr.get();
        impl->watchPoll[fd] = std::move(watchPtr);

        if (uv_poll_init(impl->loop, &watch->poll, fd)) {
            throw std::runtime_error("Failed to init poll on file descriptor.");
        }
    } else {
        watch = watchPollIter->second.get();
    }

    watch->poll.data = watch;
    watch->fd = fd;
    watch->eventCallback = std::move(callback);

    int pollEvent = 0;
    switch (event) {
    case Event::Read:
        pollEvent = UV_READABLE;
        break;
    case Event::Write:
        pollEvent = UV_WRITABLE;
        break;
    case Event::ReadWrite:
        pollEvent = UV_READABLE | UV_WRITABLE;
        break;
    default:
        throw std::runtime_error("Unhandled event.");
    }

    if (uv_poll_start(&watch->poll, pollEvent, &Watch::onEvent)) {
        throw std::runtime_error("Failed to start poll on file descriptor.");
    }
}

void RunLoop::removeWatch(int fd) {
    MBGL_VERIFY_THREAD(tid);

    auto watchPollIter = impl->watchPoll.find(fd);
    if (watchPollIter == impl->watchPoll.end()) {
        return;
    }

    Watch* watch = watchPollIter->second.release();
    impl->watchPoll.erase(watchPollIter);

    watch->closeCallback = [watch] {
        delete watch;
    };

    if (uv_poll_stop(&watch->poll)) {
        throw std::runtime_error("Failed to stop poll on file descriptor.");
    }

    uv_close(reinterpret_cast<uv_handle_t*>(&watch->poll), &Watch::onClose);
}

} // namespace util
} // namespace mbgl
