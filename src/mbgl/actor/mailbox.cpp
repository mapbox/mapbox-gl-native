#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/scheduler.hpp>

#include <cassert>

namespace mbgl {

Mailbox::Mailbox() = default;

Mailbox::Mailbox(Scheduler& scheduler_) : weakScheduler(scheduler_.makeWeakPtr()) {}

void Mailbox::open(Scheduler& scheduler_) {
    assert(!weakScheduler);

    // As with close(), block until neither receive() nor push() are in progress, and acquire the two
    // mutexes in the same order.
    std::lock_guard<std::recursive_mutex> receivingLock(receivingMutex);
    std::lock_guard<std::mutex> pushingLock(pushingMutex);

    weakScheduler = scheduler_.makeWeakPtr();

    if (closed) {
        return;
    }
    
    if (!queue.empty()) {
        auto guard = weakScheduler.lock();
        if (weakScheduler) weakScheduler->schedule(makeClosure(shared_from_this()));
    }
}

void Mailbox::close() {
    // Block until neither receive() nor push() are in progress. Two mutexes are used because receive()
    // must not block send(). Of the two, the receiving mutex must be acquired first, because that is
    // the order that an actor will obtain them when it self-sends a message, and consistent lock
    // acquisition order prevents deadlocks.
    // The receiving mutex is recursive to allow a mailbox (and thus the actor) to close itself.
    std::lock_guard<std::recursive_mutex> receivingLock(receivingMutex);
    std::lock_guard<std::mutex> pushingLock(pushingMutex);

    closed = true;
}

bool Mailbox::isOpen() const {
    return bool(weakScheduler);
}

void Mailbox::push(std::unique_ptr<Message> message) {
    std::lock_guard<std::mutex> pushingLock(pushingMutex);

    if (closed) {
        return;
    }

    std::lock_guard<std::mutex> queueLock(queueMutex);
    bool wasEmpty = queue.empty();
    queue.push(std::move(message));
    auto guard = weakScheduler.lock();
    if (wasEmpty && weakScheduler) {
        weakScheduler->schedule(makeClosure(shared_from_this()));
    }
}

void Mailbox::receive() {
    std::lock_guard<std::recursive_mutex> receivingLock(receivingMutex);

    auto guard = weakScheduler.lock();
    assert(weakScheduler);

    if (closed) {
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
        weakScheduler->schedule(makeClosure(shared_from_this()));
    }
}

// static
void Mailbox::maybeReceive(std::weak_ptr<Mailbox> mailbox) {
    if (auto locked = mailbox.lock()) {
        locked->receive();
    }
}

// static
std::function<void()> Mailbox::makeClosure(std::weak_ptr<Mailbox> mailbox) {
    return [mailbox]() { maybeReceive(mailbox); };
}

} // namespace mbgl
