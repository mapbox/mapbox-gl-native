#include "node_thread_pool.hpp"
#include "util/async_queue.hpp"

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/platform/background_scheduler.hpp>

namespace node_mbgl {

NodeThreadPool::NodeThreadPool()
    : queue(new util::AsyncQueue<std::weak_ptr<mbgl::Mailbox>>(uv_default_loop(), [](std::weak_ptr<mbgl::Mailbox> mailbox) {
        Worker* worker = new Worker(mailbox);
        Nan::AsyncQueueWorker(worker);
    })) {
    // Don't keep the event loop alive.
    queue->unref();
}

NodeThreadPool::~NodeThreadPool() {
    queue->stop();
}

void NodeThreadPool::schedule(std::weak_ptr<mbgl::Mailbox> mailbox) {
    queue->send(std::move(mailbox));
}

NodeThreadPool::Worker::Worker(std::weak_ptr<mbgl::Mailbox> mailbox_)
    : AsyncWorker(nullptr),
    mailbox(std::move(mailbox_)) {};

void NodeThreadPool::Worker::Execute() {
    mbgl::Mailbox::maybeReceive(mailbox);
}

void NodeThreadPool::Worker::WorkComplete() {
    // no-op to avoid calling nullptr callback
}

} // namespace node_mbgl

namespace mbgl {
namespace platform {

Scheduler& GetBackgroundScheduler() {
    static node_mbgl::NodeThreadPool pool;
    return pool;
}

} // namespace platform
} // namespace mbgl
