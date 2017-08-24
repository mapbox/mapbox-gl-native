#include <mbgl/util/scheduled_timer.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

namespace mbgl {


class ScheduledTimer::Impl {
public:
    // Will be called on the timer thread so it has access to the timer loop
    void start(mbgl::Duration timeout, ActorRef<ScheduledTimer> task) {
        // Create the timer on this thread and start it
        timer = std::make_unique<util::Timer>();
        timer->start(timeout, Duration::zero(), [task=std::move(task)]() mutable {
            task.invoke(&ScheduledTimer::onTimer);
        });
    }

    void cancel() {
        if (timer) {
            timer->stop();
        }
    }
    
private:
    std::unique_ptr<util::Timer> timer;
};
    
ScheduledTimer::ScheduledTimer(Scheduler& timerScheduler,
                               mbgl::Duration timeout,
                               std::weak_ptr<Mailbox> destMailbox_,
                               std::unique_ptr<Message> message_)
    : mailbox(std::make_shared<Mailbox>(*Scheduler::GetCurrent()))
    , impl(std::make_unique<Actor<Impl>>(timerScheduler))
    , destMailbox(std::move(destMailbox_))
    , message(std::move(message_)) {

    // Let the timer start on the timer scheduler
    impl->invoke(&Impl::start, timeout, ActorRef<ScheduledTimer>(*this, mailbox));
}
    
ScheduledTimer::~ScheduledTimer() {
    impl->invoke(&ScheduledTimer::Impl::cancel);
    mailbox->close();
};

bool ScheduledTimer::isFinished() {
    return finished;
}

void ScheduledTimer::onTimer() {
    if (!message) return;
    
    finished = true;
    if (auto mb = destMailbox.lock()) {
        mb->push(std::move(message));
    }
}

} // namespace mbgl
