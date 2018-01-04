#pragma once

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>

#include <atomic>
#include <functional>
#include <utility>
#include <queue>
#include <mutex>

namespace mbgl {
namespace util {

using LOOP_HANDLE = void *;

class RunLoop : public Scheduler,
                private util::noncopyable {
public:
    enum class Type : uint8_t {
        Default,
        New,
    };

    enum class Event : uint8_t {
        None      = 0,
        Read      = 1,
        Write     = 2,
        ReadWrite = Read | Write,
    };

    RunLoop(Type type = Type::Default);
    ~RunLoop() override;

    static RunLoop* Get();
    static LOOP_HANDLE getLoopHandle();

    void run();
    void runOnce();
    void stop();

    // So far only needed by the libcurl backend.
    void addWatch(int fd, Event, std::function<void(int, Event)>&& callback);
    void removeWatch(int fd);

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        std::shared_ptr<WorkTask> task = WorkTask::make(std::forward<Fn>(fn), std::forward<Args>(args)...);
        push(task);
    }

    // Post the cancellable work fn(args...) to this RunLoop.
    template <class Fn, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeCancellable(Fn&& fn, Args&&... args) {
        std::shared_ptr<WorkTask> task = WorkTask::make(std::forward<Fn>(fn), std::forward<Args>(args)...);
        push(task);
        return std::make_unique<WorkRequest>(task);
    }
                    
    void schedule(std::weak_ptr<Mailbox> mailbox) override {
        invoke([mailbox] () {
            Mailbox::maybeReceive(mailbox);
        });
    }

    class Impl;

private:
    MBGL_STORE_THREAD(tid)

    using Queue = std::queue<std::shared_ptr<WorkTask>>;

    void push(std::shared_ptr<WorkTask>);

    void withMutex(std::function<void()>&& fn) {
        std::lock_guard<std::mutex> lock(mutex);
        fn();
    }

    void process() {
        Queue queue_;
        withMutex([&] { queue_.swap(queue); });

        while (!queue_.empty()) {
            (*(queue_.front()))();
            queue_.pop();
        }
    }

    Queue queue;
    std::mutex mutex;

    std::unique_ptr<Impl> impl;
};

} // namespace util
} // namespace mbgl

#include <mbgl/util/work_task_impl.hpp>
