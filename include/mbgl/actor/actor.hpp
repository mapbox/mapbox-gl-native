#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <future>
#include <type_traits>

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

    Construction and destruction of an actor is currently synchronous: the corresponding `O`
    object is constructed synchronously by the `Actor` constructor, and destructed synchronously
    by the `~Actor` destructor, after ensuring that the `O` is not currently receiving an
    asynchronous message. (Construction and destruction may change to be asynchronous in the
    future.) The constructor of `O` is passed an `ActorRef<O>` referring to itself (which it
    can use to self-send messages), followed by the forwarded arguments passed to `Actor<O>`.

    Please don't send messages that contain shared pointers or references. That subverts the
    purpose of the actor model: prohibiting direct concurrent access to shared state.
*/


namespace util {
template <class> class Thread;
} // namespace util

template <class Object>
class Actor : public util::noncopyable {
public:

    // Enabled for Objects with a constructor taking ActorRef<Object> as the first parameter
    template <typename U = Object, class... Args, typename std::enable_if<std::is_constructible<U, ActorRef<U>, Args...>::value>::type * = nullptr>
    Actor(Scheduler& scheduler, Args&&... args_)
            : mailbox(std::make_shared<Mailbox>(scheduler)),
              object(self(), std::forward<Args>(args_)...) {
    }

    // Enabled for plain Objects
    template<typename U = Object, class... Args, typename std::enable_if<!std::is_constructible<U, ActorRef<U>, Args...>::value>::type * = nullptr>
    Actor(Scheduler& scheduler, Args&& ... args_)
            : mailbox(std::make_shared<Mailbox>(scheduler)), object(std::forward<Args>(args_)...) {
    }

    ~Actor() {
        mailbox->close();
    }

    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        mailbox->push(actor::makeMessage(object, fn, std::forward<Args>(args)...));
    }

    template <typename Fn, class... Args>
    auto ask(Fn fn, Args&&... args) {
        // Result type is deduced from the function's return type
        using ResultType = typename std::result_of<decltype(fn)(Object, Args...)>::type;

        std::promise<ResultType> promise;
        auto future = promise.get_future();
        mailbox->push(actor::makeMessage(std::move(promise), object, fn, std::forward<Args>(args)...));
        return future;
    }

    ActorRef<std::decay_t<Object>> self() {
        return ActorRef<std::decay_t<Object>>(object, mailbox);
    }

    operator ActorRef<std::decay_t<Object>>() {
        return self();
    }

private:
    template<typename U>
    friend class util::Thread;

    std::shared_ptr<Mailbox> mailbox;
    Object object;
};

} // namespace mbgl
