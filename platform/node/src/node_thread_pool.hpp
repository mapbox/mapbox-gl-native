#pragma once

#include <mbgl/actor/scheduler.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

namespace util { template <typename T> class AsyncQueue; }

class NodeThreadPool : public mbgl::Scheduler {
public:
    NodeThreadPool();
    ~NodeThreadPool();

    void schedule(std::weak_ptr<mbgl::Mailbox>) override;
    
    std::unique_ptr<Scheduled> schedule(mbgl::Duration, std::weak_ptr<mbgl::Mailbox>, std::unique_ptr<mbgl::Message>) override {
        // Not implemented
        assert(false);
        return {};
    };

private:
    util::AsyncQueue<std::weak_ptr<mbgl::Mailbox>>* queue;

    class Worker : public Nan::AsyncWorker {
    public:
        Worker(std::weak_ptr<mbgl::Mailbox>);

        void Execute();
        void WorkComplete();

    private:
        std::weak_ptr<mbgl::Mailbox> mailbox;
    };
};

} // namespace node_mbgl
