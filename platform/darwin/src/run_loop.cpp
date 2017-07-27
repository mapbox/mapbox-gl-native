#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/scheduled_timer.hpp>

#include <CoreFoundation/CoreFoundation.h>

namespace mbgl {
namespace util {

class RunLoop::Impl {
public:
    std::unique_ptr<AsyncTask> async;
};

RunLoop* RunLoop::Get() {
    assert(dynamic_cast<RunLoop*>(Scheduler::GetCurrent()));
    return static_cast<RunLoop*>(Scheduler::GetCurrent());
}

RunLoop::RunLoop(Type)
  : impl(std::make_unique<Impl>()) {
    assert(!Scheduler::GetCurrent());
    Scheduler::SetCurrent(this);
    impl->async = std::make_unique<AsyncTask>(std::bind(&RunLoop::process, this));
}

RunLoop::~RunLoop() {
    assert(Scheduler::GetCurrent());
    Scheduler::SetCurrent(nullptr);
}

void RunLoop::push(std::shared_ptr<WorkTask> task) {
    withMutex([&] { queue.push(std::move(task)); });
    impl->async->send();
}

void RunLoop::run() {
    CFRunLoopRun();
}

void RunLoop::runOnce() {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false);
}

void RunLoop::stop() {
    invoke([&] { CFRunLoopStop(CFRunLoopGetCurrent()); });
}
    
std::unique_ptr<Scheduler::Scheduled> RunLoop::schedule(Duration timeout, std::weak_ptr<Mailbox> mailbox, std::unique_ptr<Message> message) {
    return std::make_unique<ScheduledTimer>(*this, timeout, std::move(mailbox), std::move(message));
}

} // namespace util
} // namespace mbgl
