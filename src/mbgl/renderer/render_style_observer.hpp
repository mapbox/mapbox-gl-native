#pragma once

#include <exception>

namespace mbgl {

class RenderStyleObserver {
public:
    virtual ~RenderStyleObserver() = default;
    virtual void onInvalidate() {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace mbgl
