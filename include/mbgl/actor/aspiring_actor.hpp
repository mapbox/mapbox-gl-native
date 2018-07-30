#pragma once

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl {

template <class Object>
class EstablishedActor;

template <class Object>
class Actor;

/*
    An `AspiringActor<O>` is one half of the pair of types that comprise an actor (see `Actor<O>`),
    the other half being `EstablishedActor<O>`.  It is responsible for:
     - ownership of the actor's `Mailbox`
     - allocating the memory for (but *not* constructing) the target object `O`
 
    Using these two pieces--the mailbox and a stable address for `O`--an `AspiringActor<O>` can
    accept messages for the target object, or provide `ActorRef<O>`s that do so, before the object
    has actually been constructed by the corresponding `EstablishedActor<O>`.  (Such messages are
    queued in the mailbox until after the object is constructed.)
 
    This allows for an `AspiringActor<O>` to be created and safely used by a thread other than the
    one on which the target object will (eventually) live.
*/
template <class Object>
class AspiringActor {
public:
    AspiringActor() : mailbox(std::make_shared<Mailbox>()) {
        // mailbox starts closed because the `Object` hasn't yet been constructed
        assert(!mailbox->isOpen());
    }
    
    AspiringActor(const AspiringActor&) = delete;
    
    ActorRef<std::decay_t<Object>> self() {
        return ActorRef<std::decay_t<Object>>(object(), mailbox);
    }
    
private:
    std::shared_ptr<Mailbox> mailbox;
    std::aligned_storage_t<sizeof(Object)> objectStorage;
    
    Object& object() {
        return *reinterpret_cast<Object *>(&objectStorage);
    }
    
    friend class EstablishedActor<Object>;
    friend class Actor<Object>;
};

} // namespace mbgl
