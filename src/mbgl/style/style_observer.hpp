#pragma once

#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/source/source_observer.hpp>

namespace mbgl {

class StyleObserver : public GlyphStoreObserver, public SpriteStoreObserver, public SourceObserver {
public:
    virtual ~StyleObserver() = default;

    /**
     * In addition to the individual glyph, sprite, and source events, the
     * following "rollup" events are provided for convenience. They are
     * strictly additive; e.g. when a source is loaded, both `onSourceLoaded`
     * and `onResourceLoaded` will be called.
     */
    virtual void onResourceLoaded() {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace mbgl
