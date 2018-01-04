#pragma once

#include <mbgl/util/optional.hpp>

#include <future>
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

template <class ResultType, class Object, class MemberFn, class ArgsTuple>
class AskMessageImpl : public Message {
public:
    AskMessageImpl(std::promise<ResultType> promise_, Object& object_, MemberFn memberFn_, ArgsTuple argsTuple_)
        : object(object_),
          memberFn(memberFn_),
          argsTuple(std::move(argsTuple_)),
          promise(std::move(promise_)) {
    }

    void operator()() override {
        promise.set_value(ask(std::make_index_sequence<std::tuple_size<ArgsTuple>::value>()));
    }

    template <std::size_t... I>
    ResultType ask(std::index_sequence<I...>) {
        return (object.*memberFn)(std::move(std::get<I>(argsTuple))...);
    }

    Object& object;
    MemberFn memberFn;
    ArgsTuple argsTuple;
    std::promise<ResultType> promise;
};

template <class Object, class MemberFn, class ArgsTuple>
class AskMessageImpl<void, Object, MemberFn, ArgsTuple> : public Message {
public:
    AskMessageImpl(std::promise<void> promise_, Object& object_, MemberFn memberFn_, ArgsTuple argsTuple_)
            : object(object_),
            memberFn(memberFn_),
            argsTuple(std::move(argsTuple_)),
            promise(std::move(promise_)) {
    }

    void operator()() override {
        ask(std::make_index_sequence<std::tuple_size<ArgsTuple>::value>());
        promise.set_value();
    }

    template <std::size_t... I>
    void ask(std::index_sequence<I...>) {
        (object.*memberFn)(std::move(std::get<I>(argsTuple))...);
    }

    Object& object;
    MemberFn memberFn;
    ArgsTuple argsTuple;
    std::promise<void> promise;
};

namespace actor {

template <class Object, class MemberFn, class... Args>
std::unique_ptr<Message> makeMessage(Object& object, MemberFn memberFn, Args&&... args) {
    auto tuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_unique<MessageImpl<Object, MemberFn, decltype(tuple)>>(object, memberFn, std::move(tuple));
}

template <class ResultType, class Object, class MemberFn, class... Args>
std::unique_ptr<Message> makeMessage(std::promise<ResultType>&& promise, Object& object, MemberFn memberFn, Args&&... args) {
    auto tuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_unique<AskMessageImpl<ResultType, Object, MemberFn, decltype(tuple)>>(std::move(promise), object, memberFn, std::move(tuple));
}

} // namespace actor
} // namespace mbgl
