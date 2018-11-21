#pragma once

#include <mbgl/actor/aspiring_actor.hpp>
#include <mbgl/actor/established_actor.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl {

/*
    An `Actor<O>` is an owning reference to an asynchronous object of type `O`: an "actor".
    Communication with an actor happens via message passing: you send a message to the object
    (using `invoke`), passing a pointer to the member function to call and arguments which
    are then forwarded to the actor.

    The actor receives messages sent to it asynchronously, in a manner defined its `Scheduler`.
    To store incoming messages before their receipt, each actor has a `Mailbox`, which acts as
    a FIFO queue. Messages sent from actor S to actor R are guaranteed to be processed in the
    order sent. However, relative order of messages sent by two *different* actors S1 and S2
    to R is *not* guaranteed (and can't be: S1 and S2 may be acting asynchronously with respect
    to each other).

    An `Actor<O>` can be converted to an `ActorRef<O>`, a non-owning value object representing
    a (weak) reference to the actor. Messages can be sent via the `Ref` as well.

    It's safe -- and encouraged -- to pass `Ref`s between actors via messages. This is how two-way
    communication and other forms of collaboration between multiple actors is accomplished.

    It's safe for a `Ref` to outlive its `Actor` -- the reference is "weak", and does not extend
    the lifetime of the owning Actor, and sending a message to a `Ref` whose `Actor` has died is
    a no-op. (In the future, a dead-letters queue or log may be implemented.)

    Construction and destruction of an Actor is synchronous: the corresponding `O`
    object is constructed synchronously by the `Actor` constructor, and destructed synchronously
    by the `~Actor` destructor, after ensuring that the `O` is not currently receiving an
    asynchronous message. The constructor of `O` is passed an `ActorRef<O>` referring to itself
    (which it can use to self-send messages), followed by the forwarded arguments passed to
    `Actor<O>`.  Asynchronous object construction can be accomplished by directly using the
    lower-level types, `AspiringActor<O>` and `EstablishedActor<O>`.

    Please don't send messages that contain shared pointers or references. That subverts the
    purpose of the actor model: prohibiting direct concurrent access to shared state.
*/
template <class Object>
class Actor {
public:
    template <class... Args>
    Actor(Scheduler& scheduler, Args&&... args)
        : target(scheduler, parent, std::forward<Args>(args)...) {}

    Actor(const Actor&) = delete;

    ActorRef<std::decay_t<Object>> self() {
        return parent.self();
    }

private:
    AspiringActor<Object> parent;
    EstablishedActor<Object> target;
};

} // namespace mbgl
