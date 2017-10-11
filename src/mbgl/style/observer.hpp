#pragma once

#include <mbgl/style/source_observer.hpp>

#include <exception>

namespace mbgl {
namespace style {

class Observer : public SourceObserver {
public:
    virtual void onStyleLoading() {}
    virtual void onStyleLoaded() {}
    virtual void onUpdate() {}
    virtual void onStyleError(std::exception_ptr) {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace style
} // namespace mbgl
