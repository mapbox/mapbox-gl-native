#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>

#include <memory>

namespace mbgl {

/*
    An `ActorRef<O>` is a *non*-owning, weak reference to an actor of type `O`. You can send it
    messages just like an `Actor<O>`. It's a value object: safe to copy and pass between actors
    via messages.

    An `ActorRef<O>` does not extend the lifetime of the corresponding `Actor<O>`. That's determined
    entirely by whichever object owns the `Actor<O>` -- the actor's "supervisor".

    It's safe for a `Ref` to outlive its `Actor` -- the reference is "weak", and does not extend
    the lifetime of the owning Actor, and sending a message to a `Ref` whose `Actor` has died is
    a no-op. (In the future, a dead-letters queue or log may be implemented.)
*/

template <class Object>
class ActorRef {
public:
    ActorRef(Object& object_, std::weak_ptr<Mailbox> weakMailbox_)
        : object(&object_),
          weakMailbox(std::move(weakMailbox_)) {
    }

    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        if (auto mailbox = weakMailbox.lock()) {
            mailbox->push(actor::makeMessage(*object, fn, std::forward<Args>(args)...));
        }
    }

    template <typename Fn, class... Args>
    auto ask(Fn fn, Args&&... args) {
        // Result type is deduced from the function's return type
        using ResultType = typename std::result_of<decltype(fn)(Object, Args...)>::type;

        std::promise<ResultType> promise;
        auto future = promise.get_future();

        if (auto mailbox = weakMailbox.lock()) {
            mailbox->push(
                    actor::makeMessage(
                            std::move(promise), *object, fn, std::forward<Args>(args)...
                    )
            );
        } else {
            promise.set_exception(std::make_exception_ptr(std::runtime_error("Actor has gone away")));
        }

        return future;
    }

private:
    Object* object;
    std::weak_ptr<Mailbox> weakMailbox;
};

} // namespace mbgl
