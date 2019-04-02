#pragma once

#include <mbgl/renderer/renderer_observer.hpp>

namespace mbgl {

class ImageManagerObserver {
public:
    virtual ~ImageManagerObserver() = default;

    virtual void onStyleImageMissing(const std::string&, std::function<void()> done) { done(); }
};

} // namespace mbgl
