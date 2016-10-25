#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <cassert>

namespace mbgl {

Mailbox::Mailbox(Scheduler& scheduler_)
    : scheduler(scheduler_) {
}

void Mailbox::push(std::unique_ptr<Message> message) {
    assert(!closing);

    std::lock_guard<std::mutex> queueLock(queueMutex);
    bool wasEmpty = queue.empty();
    queue.push(std::move(message));
    if (wasEmpty) {
        scheduler.schedule(shared_from_this());
    }
}

void Mailbox::close() {
    // Block until the scheduler is guaranteed not to be executing receive().
    std::lock_guard<std::mutex> closingLock(closingMutex);
    closing = true;
}

void Mailbox::receive() {
    std::lock_guard<std::mutex> closingLock(closingMutex);

    if (closing) {
        return;
    }

    std::unique_ptr<Message> message;
    bool wasEmpty;

    {
        std::lock_guard<std::mutex> queueLock(queueMutex);
        assert(!queue.empty());
        message = std::move(queue.front());
        queue.pop();
        wasEmpty = queue.empty();
    }

    (*message)();

    if (!wasEmpty) {
        scheduler.schedule(shared_from_this());
    }
}

void Mailbox::maybeReceive(std::weak_ptr<Mailbox> mailbox) {
    if (auto locked = mailbox.lock()) {
        locked->receive();
    }
}

} // namespace mbgl
