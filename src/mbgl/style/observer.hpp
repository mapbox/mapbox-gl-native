#pragma once

#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/map/update.hpp>

namespace mbgl {
namespace style {

class Observer : public GlyphStoreObserver,
                 public SpriteStoreObserver,
                 public SourceObserver {
public:
    virtual void onUpdate(Update) {}
    virtual void onStyleError() {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace style
} // namespace mbgl
