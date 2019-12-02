#pragma once

#include <mapbox/weak.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class Mailbox;

// Using this type as a return type enforces the client to retain the returned object.
// TODO:  Move to a separate file if/when other clients for this aux API turn up.
template <typename T>
class Pass {
public:
    Pass(T&& obj_) : obj(std::forward<T>(obj_)) {}
    Pass(Pass&&) = default;
    Pass(const Pass&) = delete;
    operator T() && { return std::move(obj); }

private:
    T obj;
};

template <typename T>
using PassRefPtr = Pass<std::shared_ptr<T>>;

/*
    A `Scheduler` is responsible for coordinating the processing of messages by
    one or more actors via their mailboxes. It's an abstract interface. Currently,
    the following concrete implementations exist:

    * `ThreadPool` can coordinate an unlimited number of actors over any number of
      threads via a pool, preserving the following behaviors:

      - Messages from each individual mailbox are processed in order
      - Only a single message from a mailbox is processed at a time; there is no
        concurrency within a mailbox

      Subject to these constraints, processing can happen on whatever thread in the
      pool is available.

    * `Scheduler::GetCurrent()` is typically used to create a mailbox and `ActorRef`
      for an object that lives on the main thread and is not itself wrapped an
      `Actor`. The underlying implementation of this Scheduler should usually be
      a `RunLoop`
        auto mailbox = std::make_shared<Mailbox>(*Scheduler::Get());
        Actor<Worker> worker(threadPool, ActorRef<Foo>(*this, mailbox));
*/
class Scheduler {
public:
    virtual ~Scheduler() = default;

    // Enqueues a function for execution.
    virtual void schedule(std::function<void()>) = 0;
    // Makes a weak pointer to this Scheduler.
    virtual mapbox::base::WeakPtr<Scheduler> makeWeakPtr() = 0;

    // Returns a closure wrapping the given one.
    //
    // When the returned closure is invoked for the first time, it schedules
    // the given closure to this scheduler, the consequent calls of the
    // returned closure are ignored.
    //
    // If this scheduler is already deleted by the time the returnded closure is
    // first invoked, the call is ignored.
    std::function<void()> bindOnce(std::function<void()>);

    // Enqueues the given |task| for execution into this scheduler's task queue and
    // then enqueues the |reply| with the captured task result to the current
    // task queue.
    //
    // The |TaskFn| return type must be compatible with the |ReplyFn| argument type.
    // Note: the task result is copied and passed by value.
    template <typename TaskFn, typename ReplyFn>
    void scheduleAndReplyValue(const TaskFn& task, const ReplyFn& reply) {
        assert(GetCurrent());
        scheduleAndReplyValue(task, reply, GetCurrent()->makeWeakPtr());
    }

    // Set/Get the current Scheduler for this thread
    static Scheduler* GetCurrent();
    static void SetCurrent(Scheduler*);

    // Get the scheduler for asynchronous tasks. This method
    // will lazily initialize a shared worker pool when ran
    // from the first time.
    // The scheduled tasks might run in parallel on different
    // threads.
    // TODO : Rename to GetPool()
    static PassRefPtr<Scheduler> GetBackground();

    // Get the *sequenced* scheduler for asynchronous tasks.
    // Unlike the method above, the returned scheduler
    // (once stored) represents a single thread, thus each
    // newly scheduled task is guarantied to run after the
    // previously scheduled one.
    //
    // Sequenced scheduler can be used for running tasks
    // on the same thread-unsafe object.
    static PassRefPtr<Scheduler> GetSequenced();

protected:
    template <typename TaskFn, typename ReplyFn>
    void scheduleAndReplyValue(const TaskFn& task,
                               const ReplyFn& reply,
                               mapbox::base::WeakPtr<Scheduler> replyScheduler) {
        auto scheduled = [replyScheduler, task, reply] {
            auto lock = replyScheduler.lock();
            if (!replyScheduler) return;
            auto scheduledReply = [reply, result = task()] { reply(result); };
            replyScheduler->schedule(std::move(scheduledReply));
        };

        schedule(std::move(scheduled));
    }
};

} // namespace mbgl
