#pragma once

#include <exception>

namespace mbgl {

class SpriteStoreObserver {
public:
    virtual ~SpriteStoreObserver() = default;

    virtual void onSpriteLoaded() {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl
