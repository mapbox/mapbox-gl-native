#pragma once

#include <exception>

namespace mbgl {
namespace style {

class Source;

class SourceObserver {
public:
    virtual ~SourceObserver() = default;

    virtual void onSourceLoaded(Source&) {}
    virtual void onSourceChanged(Source&) {}
    virtual void onSourceError(Source&, std::exception_ptr) {}

    // Source description needs to be reloaded
    virtual void onSourceDescriptionChanged(Source&) {}
};

} // namespace style
} // namespace mbgl
