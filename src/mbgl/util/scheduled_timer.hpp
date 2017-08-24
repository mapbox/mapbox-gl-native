#pragma once

#include <mbgl/actor/actor.hpp>

#include <memory>

namespace mbgl {
    
class Mailbox;
class Message;
class Scheduler;
    
namespace util {
class RunLoop;
} // namespace util

// Bridges to the dedicated timer thread
class ScheduledTimer : public Scheduler::Scheduled {
public:
    
    ScheduledTimer(Scheduler& timerScheduler,
                   mbgl::Duration timeout,
                   std::weak_ptr<Mailbox> destMailbox,
                   std::unique_ptr<Message> message);
    
    ~ScheduledTimer() override;
    
    bool isFinished() override;
    
private:
    void onTimer();
    
    // The Timer's own mailbox
    std::shared_ptr<Mailbox> mailbox;
    
    class Impl;
    friend class Impl;
    std::unique_ptr<Actor<Impl>> impl;
    
    // The destination Mailbox and message
    // to deliver
    std::weak_ptr<Mailbox> destMailbox;
    std::unique_ptr<Message> message;
    
    std::atomic<bool> finished { false };
};

} // namespace mbgl
