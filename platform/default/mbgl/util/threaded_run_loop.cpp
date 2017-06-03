#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/threaded_run_loop.hpp>

namespace mbgl {

ThreadedRunLoop::ThreadedRunLoop(const std::string& name)
    : thread([&]() {
        platform::setCurrentThreadName(name);

        loop = std::make_unique<util::RunLoop>(util::RunLoop::Type::New);
        running.set_value();

        loop->run();
        loop.reset();
    }) {
    running.get_future();
}

ThreadedRunLoop::~ThreadedRunLoop() {
    loop->stop();
    thread.join();
}

void ThreadedRunLoop::schedule(std::weak_ptr<Mailbox> mailbox) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(mailbox);
    }

    loop->invoke([this] { receive(); });
}

void ThreadedRunLoop::receive() {
    std::unique_lock<std::mutex> lock(mutex);

    auto mailbox = queue.front();
    queue.pop();
    lock.unlock();

    Mailbox::maybeReceive(mailbox);
}

} // namespace mbgl
