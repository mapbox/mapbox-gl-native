#pragma once

#include <mbgl/util/event.hpp>

#include <exception>

namespace mbgl {
namespace style {

class Source;

class SourceObserver {
public:
    virtual ~SourceObserver() = default;

    virtual void onSourceLoaded(Source&) {}
    virtual void onSourceChanged(Source&) {}
    virtual void onSourceError(Source&, std::exception_ptr, EventSeverity = EventSeverity::Error) {}

    // Source description needs to be reloaded
    virtual void onSourceDescriptionChanged(Source&) {}
};

} // namespace style
} // namespace mbgl
