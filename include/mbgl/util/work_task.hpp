#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <memory>

namespace mbgl {

// A movable type-erasing function wrapper. This allows to store arbitrary invokable
// things (like std::function<>, or the result of a movable-only std::bind()) in the queue.
// Source: http://stackoverflow.com/a/29642072/331379
class WorkTask : private util::noncopyable {
public:
    virtual ~WorkTask() = default;

    virtual void operator()() = 0;
    virtual void cancel() = 0;

    template <class Fn, class... Args>
    static std::shared_ptr<WorkTask> make(Fn&&, Args&&...);
};

} // namespace mbgl
