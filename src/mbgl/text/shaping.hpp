#ifndef MBGL_TEXT_SHAPING
#define MBGL_TEXT_SHAPING

#include <mbgl/text/glyph.hpp>

#include <mbgl/util/vec.hpp>

namespace mbgl {

    class PositionedIcon {
        public:
            inline explicit PositionedIcon() {}
            inline explicit PositionedIcon(Rect<uint16_t> _image,
                    float _top, float _bottom, float _left, float _right) :
                image(_image), top(_top), bottom(_bottom), left(_left), right(_right) {}
            Rect<uint16_t> image;
            float top = 0;
            float bottom = 0;
            float left = 0;
            float right = 0;

            operator bool() const { return image.hasArea(); }
    };

    class SymbolLayoutProperties;

    PositionedIcon shapeIcon(const Rect<uint16_t>& image, const SymbolLayoutProperties&);

}

#endif
