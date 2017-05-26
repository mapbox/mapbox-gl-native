#pragma once

#include <mbgl/actor/scheduler.hpp>

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace mbgl {

class ThreadedRunLoop : public Scheduler {
public:
    ThreadedRunLoop(const std::string& name);
    ~ThreadedRunLoop() override;

    void schedule(std::weak_ptr<Mailbox>) override;

private:
    void receive();

    std::mutex mutex;
    std::promise<void> running;
    std::queue<std::weak_ptr<Mailbox>> queue;
    std::thread thread;
    std::unique_ptr<util::RunLoop> loop;
};

} // namespace mbgl
