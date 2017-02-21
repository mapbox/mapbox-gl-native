#pragma once

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

    * `RunLoop` is a `Scheduler` that is typically used to create a mailbox and
      `ActorRef` for an object that lives on the main thread and is not itself wrapped
      as an `Actor`:

        auto mailbox = std::make_shared<Mailbox>(*util::RunLoop::Get());
        Actor<Worker> worker(threadPool, ActorRef<Foo>(*this, mailbox));
*/

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void schedule(std::weak_ptr<Mailbox>) = 0;
};

} // namespace mbgl
