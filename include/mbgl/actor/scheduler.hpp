#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/async_request.hpp>

#include <memory>

namespace mbgl {

class Mailbox;
class Message;

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
    virtual void schedule(std::weak_ptr<Mailbox>) = 0;

    /*
     A token to the scheduled action, at the end of
     the token's life-time, the scheduled action is
     canceled.
    */
    class Scheduled : public AsyncRequest {
    public:
        ~Scheduled() override = default;
        
        virtual bool isFinished() = 0;
    };
    
    // Schedule message delivery at a later time
    virtual std::unique_ptr<Scheduled> schedule(Duration timeout, std::weak_ptr<Mailbox>, std::unique_ptr<Message>) = 0;
    
    // Set/Get the current Scheduler for this thread
    static Scheduler* GetCurrent();
    static void SetCurrent(Scheduler*);
};

} // namespace mbgl
