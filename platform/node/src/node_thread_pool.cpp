#include "node_thread_pool.hpp"
#include "util/async_queue.hpp"

#include <mbgl/actor/mailbox.hpp>

namespace node_mbgl {

NodeThreadPool::NodeThreadPool()
    : queue(new util::AsyncQueue<std::weak_ptr<mbgl::Mailbox>>(uv_default_loop(), [this](std::weak_ptr<mbgl::Mailbox> mailbox) {
        NodeThread* worker = new NodeThread(mailbox);
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

NodeThreadPool::NodeThread::NodeThread(std::weak_ptr<mbgl::Mailbox> mailbox_)
    : AsyncWorker(nullptr),
    mailbox(std::move(mailbox_)) {};

void NodeThreadPool::NodeThread::Execute() {
    mbgl::Mailbox::maybeReceive(mailbox);
}

void NodeThreadPool::NodeThread::WorkComplete() {
    // no-op to avoid calling nullptr callback
}

} // namespace node_mbgl
