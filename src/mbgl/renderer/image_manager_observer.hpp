#pragma once

#include <mbgl/renderer/renderer_observer.hpp>

namespace mbgl {

class ImageManagerObserver {
public:
    virtual ~ImageManagerObserver() = default;

    using StyleImageMissingCallback = std::function<void()>;
    virtual void onStyleImageMissing(const std::string&, RendererObserver::StyleImageMissingCallback done) { done(); }
};

} // namespace mbgl
