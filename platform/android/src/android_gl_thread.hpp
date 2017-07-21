#pragma once

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/renderer.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace mbgl {
namespace android {

class AndroidGLThread : public Scheduler {
public:
    using InvalidateCallback = std::function<void ()>;

    template <class... Args>
    AndroidGLThread(InvalidateCallback callback_, Args&&... args)
        : renderer(std::make_unique<Renderer>(std::forward<Args>(args)...))
        , mailbox(std::make_shared<Mailbox>(*this))
        , callback(callback_)
        , rendererRef(*renderer, mailbox) {
    }

    ~AndroidGLThread() override = default;

    ActorRef<Renderer> actor() const {
        return rendererRef;
    }

    void schedule(std::weak_ptr<Mailbox> scheduled) override {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(scheduled);
        }

        // Invalidate so we get processing time later
        callback();
    }

    // Only safe on the GL Thread
    void process() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);

            if (queue.empty()) {
                return;
            }

            auto scheduled = queue.front();
            queue.pop();

            lock.unlock();

            Mailbox::maybeReceive(scheduled);
        }
    }

    // Only safe to access on the GL Thread
    std::unique_ptr<Renderer> renderer;

private:
    std::mutex mutex;
    std::queue<std::weak_ptr<Mailbox>> queue;

    std::shared_ptr<Mailbox> mailbox;
    InvalidateCallback callback;
    ActorRef<Renderer> rendererRef;
};

} // namespace android
} // namespace mbgl
