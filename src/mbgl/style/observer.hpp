#pragma once

#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/style/source_observer.hpp>

namespace mbgl {
namespace style {

class Observer : public GlyphStoreObserver,
                 public SpriteStoreObserver,
                 public SourceObserver {
public:
    /**
     * In addition to the individual glyph, sprite, and source events, the
     * following "rollup" events are provided for convenience. They are
     * strictly additive; e.g. when a source is loaded, both `onSourceLoaded`
     * and `onNeedsRepaint` will be called.
     */
    void onNeedsRepaint() override {}
    virtual void onStyleError() {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace style
} // namespace mbgl
