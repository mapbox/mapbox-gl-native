#pragma once

#include <exception>

namespace mbgl {

class SpriteAtlasObserver {
public:
    virtual ~SpriteAtlasObserver() = default;

    virtual void onSpriteLoaded() {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
