#pragma once

#include <memory>
#include <mutex>
#include <queue>

namespace mbgl {

class Scheduler;
class Message;

class Mailbox : public std::enable_shared_from_this<Mailbox> {
public:
    Mailbox(Scheduler&);

    void push(std::unique_ptr<Message>);

    void close();
    void receive();

    static void maybeReceive(std::weak_ptr<Mailbox>);

private:
    Scheduler& scheduler;

    std::mutex closingMutex;
    bool closing { false };

    std::mutex queueMutex;
    std::queue<std::unique_ptr<Message>> queue;
};

} // namespace mbgl
