#pragma once

#include <mbgl/util/optional.hpp>

#include <memory>
#include <mutex>
#include <queue>

namespace mbgl {

class Scheduler;
class Message;

class Mailbox : public std::enable_shared_from_this<Mailbox> {
public:
   
    // Create a "holding" mailbox, messages to which will remain queued,
    // unconsumed, until the mailbox is associated with a Scheduler using
    // start(). This allows a Mailbox object to be created on one thread and
    // later transferred to a different target thread that may not yet exist.
    Mailbox();
    
    Mailbox(Scheduler&);

    // Attach the given scheduler to this mailbox and begin processing messages
    // sent to it. The mailbox must be a "holding" mailbox, as created by the
    // default constructor Mailbox().
    void open(Scheduler& scheduler_);
    void close();

    bool isOpen() const;

    void push(std::unique_ptr<Message>);
    void receive();

    static void maybeReceive(std::weak_ptr<Mailbox>);

private:
    optional<Scheduler*> scheduler;

    std::recursive_mutex receivingMutex;
    std::mutex pushingMutex;

    bool closed { false };

    std::mutex queueMutex;
    std::queue<std::unique_ptr<Message>> queue;
};

} // namespace mbgl
