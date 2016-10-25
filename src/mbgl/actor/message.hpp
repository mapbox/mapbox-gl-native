#pragma once

#include <utility>

namespace mbgl {

// A movable type-erasing function wrapper. This allows to store arbitrary invokable
// things (like std::function<>, or the result of a movable-only std::bind()) in the queue.
// Source: http://stackoverflow.com/a/29642072/331379
class Message {
public:
    virtual ~Message() = default;
    virtual void operator()() = 0;
};

template <class Object, class MemberFn, class ArgsTuple>
class MessageImpl : public Message {
public:
    MessageImpl(Object& object_, MemberFn memberFn_, ArgsTuple argsTuple_)
      : object(object_),
        memberFn(memberFn_),
        argsTuple(std::move(argsTuple_)) {
    }

    void operator()() override {
        invoke(std::make_index_sequence<std::tuple_size<ArgsTuple>::value>());
    }

    template <std::size_t... I>
    void invoke(std::index_sequence<I...>) {
        (object.*memberFn)(std::move(std::get<I>(argsTuple))...);
    }

    Object& object;
    MemberFn memberFn;
    ArgsTuple argsTuple;
};

namespace actor {

template <class Object, class MemberFn, class... Args>
std::unique_ptr<Message> makeMessage(Object& object, MemberFn memberFn, Args&&... args) {
    auto tuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_unique<MessageImpl<Object, MemberFn, decltype(tuple)>>(object, memberFn, std::move(tuple));
}

} // namespace actor
} // namespace mbgl
