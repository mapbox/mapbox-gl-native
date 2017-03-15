#pragma once

#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/sprite/sprite_atlas_observer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/map/update.hpp>

#include <exception>

namespace mbgl {
namespace style {

class Observer : public GlyphAtlasObserver,
                 public SpriteAtlasObserver,
                 public SourceObserver {
public:
    virtual void onUpdate(Update) {}
    virtual void onStyleError(std::exception_ptr) {}
    virtual void onStyleLoaded() {}
    virtual void onResourceError(std::exception_ptr) {}
};

} // namespace style
} // namespace mbgl
