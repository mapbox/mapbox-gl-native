#include "run_loop_impl.hpp"

#include <android/looper.h>

namespace {

using namespace mbgl::util;

int looperCallbackReadEvent(int fd, int, void* data) {
    auto runLoopImpl = reinterpret_cast<RunLoop::Impl*>(data);

    auto it = runLoopImpl->readPoll.find(fd);
    if (it != runLoopImpl->readPoll.end()) {
        assert(it->second);
        it->second(fd, RunLoop::Event::Read);
    }
    return 1;
}

} // namespace

namespace mbgl {
namespace util {

void RunLoop::addWatch(int fd, Event event, std::function<void(int, Event)>&& cb) {
    MBGL_VERIFY_THREAD(tid);

    if (event == Event::Read) {
        impl->readPoll[fd] = std::move(cb);

        ALooper* looper = ALooper_forThread();
        ALooper_addFd(
            looper, fd, ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT, looperCallbackReadEvent, this->impl.get());
    }
}

void RunLoop::removeWatch(int fd) {
    MBGL_VERIFY_THREAD(tid);

    auto it = impl->readPoll.find(fd);
    if (it != impl->readPoll.end()) {
        impl->readPoll.erase(it);

        ALooper* looper = ALooper_forThread();
        ALooper_removeFd(looper, fd);
    }
}

} // namespace util
} // namespace mbgl
