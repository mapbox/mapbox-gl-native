#ifndef MBGL_TEXT_GLYPH_STORE_OBSERVER
#define MBGL_TEXT_GLYPH_STORE_OBSERVER

#include <mbgl/style/types.hpp>
#include <mbgl/text/glyph_range.hpp>

#include <exception>

namespace mbgl {

class GlyphStoreObserver {
public:
    virtual ~GlyphStoreObserver() = default;

    virtual void onGlyphsLoaded(const FontStack&, const GlyphRange&) {}
    virtual void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) {}
};

} // namespace mbgl

#endif
