#pragma once

#include <mapbox/weak.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class Mailbox;

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

    // Set/Get the current Scheduler for this thread
    static Scheduler* GetCurrent();
    static void SetCurrent(Scheduler*);

    // Get the scheduler for asynchronous tasks. This method
    // will lazily initialize a shared worker pool when ran
    // from the first time.
    static std::shared_ptr<Scheduler> GetBackground();
};

} // namespace mbgl
