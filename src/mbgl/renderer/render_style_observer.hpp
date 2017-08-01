#pragma once

#include <mbgl/util/event.hpp>

#include <exception>

namespace mbgl {

class RenderStyleObserver {
public:
    virtual ~RenderStyleObserver() = default;
    virtual void onInvalidate() {}
    virtual void onResourceError(std::exception_ptr, EventSeverity = EventSeverity::Error) {}
};

} // namespace mbgl
