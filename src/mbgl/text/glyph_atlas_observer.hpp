#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/util/font_stack.hpp>

#include <exception>

namespace mbgl {

class GlyphAtlasObserver {
public:
    virtual ~GlyphAtlasObserver() = default;

    virtual void onGlyphsLoaded(const FontStack&, const GlyphRange&) {}
    virtual void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) {}
};

} // namespace mbgl
