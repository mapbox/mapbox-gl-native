#include <mbgl/platform/platform_run_loop.hpp>

#include <uv.h>

#include <cassert>
#include <functional>
#include <unordered_map>

namespace {

void dummyCallback(uv_async_t*) {}

} // namespace

namespace mbgl {

struct Watch {
    static void onEvent(uv_poll_t* poll, int, int event) {
        auto watch = reinterpret_cast<Watch*>(poll->data);

        PlatformRunLoop::Event watchEvent = PlatformRunLoop::Event::None;
        switch (event) {
        case UV_READABLE:
            watchEvent = PlatformRunLoop::Event::Read;
            break;
        case UV_WRITABLE:
            watchEvent = PlatformRunLoop::Event::Write;
            break;
        case UV_READABLE | UV_WRITABLE:
            watchEvent = PlatformRunLoop::Event::ReadWrite;
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

    std::function<void(int, PlatformRunLoop::Event)> eventCallback;
    std::function<void()> closeCallback;
};

RunLoop* PlatformRunLoop::Get() {
    assert(static_cast<RunLoop*>(Scheduler::GetCurrent()));
    return static_cast<RunLoop*>(Scheduler::GetCurrent());
}

class DefaultRunLoop : public PlatformRunLoop {
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

    PlatformRunLoop::Type type;
    std::unique_ptr<AsyncTask> async;

    std::unordered_map<int, std::unique_ptr<Watch>> watchPoll;
};

PlatformRunLoop::RunLoop(Type type) : impl(std::make_unique<Impl>()) {
    switch (type) {
    case Type::New:
        loop = new uv_loop_t;
        if (uv_loop_init(loop) != 0) {
            throw std::runtime_error("Failed to initialize loop.");
        }
        break;
    case Type::Default:
        loop = uv_default_loop();
        break;
    }

    // Just for holding a ref to the main loop and keep
    // it alive as required by libuv.
    if (uv_async_init(loop, holder, dummyCallback) != 0) {
        throw std::runtime_error("Failed to initialize async.");
    }

    type = type;

    Scheduler::SetCurrent(this);
    async = std::make_unique<AsyncTask>(std::bind(&PlatformRunLoop::process, this));
}

PlatformRunLoop::~RunLoop() {
    Scheduler::SetCurrent(nullptr);

    // Close the dummy handle that we have
    // just to keep the main loop alive.
    closeHolder();

    if (type == Type::Default) {
        return;
    }

    // Run the loop again to ensure that async
    // close callbacks have been called. Not needed
    // for the default main loop because it is only
    // closed when the application exits.
    async.reset();
    runOnce();

    if (uv_loop_close(loop) == UV_EBUSY) {
        assert(false && "Failed to close loop.");
    }
    delete loop;
}

LOOP_HANDLE PlatformRunLoop::getLoopHandle() {
    return Get()->loop;
}

void PlatformRunLoop::wake() {
    async->send();
}

void PlatformRunLoop::run() {
    uv_ref(holderHandle());
    uv_run(loop, UV_RUN_DEFAULT);
}

void PlatformRunLoop::runOnce() {
    uv_run(loop, UV_RUN_NOWAIT);
}

void PlatformRunLoop::stop() {
    invoke([&] { uv_unref(holderHandle()); });
}

void PlatformRunLoop::addWatch(int fd, Event event, std::function<void(int, Event)>&& callback) {
    Watch *watch = nullptr;
    auto watchPollIter = watchPoll.find(fd);

    if (watchPollIter == watchPoll.end()) {
        std::unique_ptr<Watch> watchPtr = std::make_unique<Watch>();

        watch = watchPtr.get();
        watchPoll[fd] = std::move(watchPtr);

        if (uv_poll_init(loop, &watch->poll, fd)) {
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

void PlatformRunLoop::removeWatch(int fd) {
    auto watchPollIter = watchPoll.find(fd);
    if (watchPollIter == watchPoll.end()) {
        return;
    }

    Watch* watch = watchPollIter->second.release();
    watchPoll.erase(watchPollIter);

    watch->closeCallback = [watch] {
        delete watch;
    };

    if (uv_poll_stop(&watch->poll)) {
        throw std::runtime_error("Failed to stop poll on file descriptor.");
    }

    uv_close(reinterpret_cast<uv_handle_t*>(&watch->poll), &Watch::onClose);
}

} // namespace mbgl
