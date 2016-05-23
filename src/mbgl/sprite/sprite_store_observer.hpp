#ifndef MBGL_SPRITE_STORE_OBSERVER
#define MBGL_SPRITE_STORE_OBSERVER

#include <exception>

namespace mbgl {

class SpriteStoreObserver {
public:
    virtual ~SpriteStoreObserver() = default;

    virtual void onSpriteLoaded() {}
    virtual void onSpriteError(std::exception_ptr) {}
};

} // namespace mbgl

#endif
