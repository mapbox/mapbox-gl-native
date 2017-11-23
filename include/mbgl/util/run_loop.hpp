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
#include <condition_variable>
#include <cassert>

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
    // This call is threadsafe.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        std::shared_ptr<WorkTask> task = WorkTask::make(std::forward<Fn>(fn), std::forward<Args>(args)...);
        push(task);
    }

    // Post the cancellable work fn(args...) to this RunLoop.
    // This call is threadsafe.
    template <class Fn, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeCancellable(Fn&& fn, Args&&... args) {
        std::shared_ptr<WorkTask> task = WorkTask::make(std::forward<Fn>(fn), std::forward<Args>(args)...);
        push(task);
        return std::make_unique<WorkRequest>(task);
    }

    // This call is threadsafe.
    void schedule(std::weak_ptr<Mailbox> mailbox) override {
        invoke([mailbox] () {
            Mailbox::maybeReceive(mailbox);
        });
    }

    // Instructs the RunLoop to stop processing tasks until a call to resume() follows. This call
    // blocks until the RunLoop finished processing the current task.
    // This call is threadsafe.
    void pause() {
        // A RunLoop can't pause itself.
        assert(static_cast<RunLoop*>(Scheduler::GetCurrent()) != this);

        std::unique_lock<std::mutex> lock(mutex);
        if (pausing) {
            // Do not attempt to pause again when we've already started the pause.
            return;
        }

        // Indicates our intent to pause this RunLoop. Just having pausing set to true doesn't mean
        // the RunLoop is paused; e.g. it might still be processing a task. We'll have to wait until
        // the process() function has confirmed that it stopped running. We do this via the running
        // variable and wait until it's set to false.
        pausing = true;

        // If we're at this point, we assume that there is no current request to pause the loop that
        // is already in progress, so running must be true.
        assert(running);
        lock.unlock();

        // Wake up the RunLoop so that it'll run process(), which eventually sets running to false.
        // We have to invoke this because otherwise, running could never be set to false, e.g. when
        // this RunLoop doesn't have any items to process.
        wake();

        // Wait until the RunLoop stops running. Once this condition is triggered, we know that the
        // RunLoop has stopped processing the current item
        lock.lock();
        cv.wait(lock, [this] { return !running; });
    }

    // Resumes processing items in this RunLoop. This call blocks until the RunLoop finished
    // processing the current task if it has not yet paused.
    // This call is threadsafe.
    void resume() {
        std::unique_lock<std::mutex> lock(mutex);
        if (!pausing || resuming) {
            // Do not attempt to resume again when we've already started to resume, or when the
            // RunLoop isn't paused
            return;
        }

        // Indicates the intent to resume this RunLoop. This is required to prevent two concurrent
        // resume() calls from two different threads from interleaving. E.g. one resume() call could
        // wait until running is set to false, then we reset to true immediately after, which leaves
        // the second resume() call waiting until it gets set to false indefinitely.
        resuming = true;

        // Make sure that a previous call to pause() (e.g. on another thread) has completed.
        cv.wait(lock, [this] { return !running; });

        // We can now reverse the pause actions.
        pausing = false;
        running = true;
        resuming = false;

        lock.unlock();

        // Finally, make sure that we resume processing the items we've collected during the pause.
        wake();
    }

    class Impl;

private:
    MBGL_STORE_THREAD(tid)

    // Wakes up the RunLoop so that it starts processing items in the queue.
    void wake();

    // Adds a WorkTask to the queue, and wakes it up.
    void push(std::shared_ptr<WorkTask> task) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(task));

            if (pausing) {
                // Do not attempt to wake up the RunLoop if we already know it's paused
                return;
            }
        }
    
        wake();
    }

    // Process items in the queue. This should be called from the thread that the RunLoop runs on.
    void process() {
        std::shared_ptr<WorkTask> task;
        std::unique_lock<std::mutex> lock(mutex);
        while (!pausing && !queue.empty()) {
            task = std::move(queue.front());
            queue.pop();
            lock.unlock();

            if (task) {
                (*task)();
            }

            lock.lock();
        }

        if (pausing && running) {
            // Notify the pause()/resume() calls that we've finished processing the current task and
            // are now truly pausing.
            running = false;

            // Use notify_all() instead of notify_one() in case we have both a pause() and a resume()
            // call waiting for this condition variable.
            cv.notify_all();
        }
    }

    std::mutex mutex;
    std::condition_variable cv;
    bool pausing = false;
    bool running = true;
    bool resuming = false;
    std::queue<std::shared_ptr<WorkTask>> queue;

    std::unique_ptr<Impl> impl;
};

} // namespace util
} // namespace mbgl

#include <mbgl/util/work_task_impl.hpp>
