#pragma once

#include <mbgl/actor/aspiring_actor.hpp>
#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl {

/*
    An `EstablishedActor<O>` is one half of the pair of types that comprise an actor (see `Actor<O>`),
    the other half being `AspiringActor<O>`.  It is responsible for managing the lifetime of the
    target object `O` and the open/closed state of the parent's `mailbox`.
 
    The `O` object's lifetime is contained by that of its owning `EstablishedActor<O>`: the 
    `EstablishedActor` constructor executes the `O` constructor via "placement new", constructing
    it at the address provided by the parent `AspiringActor`, and the `~EstablishedActor` destructor
    similarly executes the `~O` destructor (after closing the mailbox). `EstablishedActor` should
    therefore live entirely on the thread intended to own `O`.
*/

template <class Object>
class EstablishedActor {
public:
    // Construct the Object from a parameter pack `args` (i.e. `Object(args...)`)
    template <typename U = Object, class... Args, typename std::enable_if<
        std::is_constructible<U, Args...>::value ||
        std::is_constructible<U, ActorRef<U>, Args...>::value
    >::type * = nullptr>
    EstablishedActor(Scheduler& scheduler, AspiringActor<Object>& parent_, Args&& ... args)
    :   parent(parent_) {
        emplaceObject(std::forward<Args>(args)...);
        parent.mailbox->open(scheduler);
    }
    
    // Construct the `Object` from a tuple containing the constructor arguments (i.e.
    // `Object(std::get<0>(args), std::get<1>(args), ...)`)
    template <class ArgsTuple, std::size_t ArgCount = std::tuple_size<std::decay_t<ArgsTuple>>::value>
    EstablishedActor(Scheduler& scheduler, AspiringActor<Object>& parent_, ArgsTuple&& args)
    :   parent(parent_) {
        emplaceObject(std::forward<ArgsTuple>(args), std::make_index_sequence<ArgCount>{});
        parent.mailbox->open(scheduler);
    }
    
    EstablishedActor(const EstablishedActor&) = delete;

    ~EstablishedActor() {
        parent.mailbox->close();
        parent.object().~Object();
    }
    
private:
    // Enabled for Objects with a constructor taking ActorRef<Object> as the first parameter
    template <typename U = Object, class... Args, typename std::enable_if<std::is_constructible<U, ActorRef<U>, Args...>::value>::type * = nullptr>
    void emplaceObject(Args&&... args_) {
        new (&parent.objectStorage) Object(parent.self(), std::forward<Args>(args_)...);
    }

    // Enabled for plain Objects
    template <typename U = Object, class... Args, typename std::enable_if<std::is_constructible<U, Args...>::value>::type * = nullptr>
    void emplaceObject(Args&&... args_) {
        new (&parent.objectStorage) Object(std::forward<Args>(args_)...);
    }
    
    // Used to expand a tuple holding the constructor arguments
    template <class ArgsTuple, std::size_t... I>
    void emplaceObject(ArgsTuple&& args, std::index_sequence<I...>) {
        emplaceObject(std::move(std::get<I>(std::forward<ArgsTuple>(args)))...);
        (void) args; // mark args as used: if it's empty tuple, it's not actually used above.
    }

    AspiringActor<Object>& parent;
};

} // namespace mbgl
