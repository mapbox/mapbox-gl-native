#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <vector>

namespace mbgl {

class Anchor;
class PositionedIcon;
enum class SymbolContent : uint8_t;

class SymbolQuad {
public:
    SymbolQuad(Point<float> tl_,
               Point<float> tr_,
               Point<float> bl_,
               Point<float> br_,
               Rect<uint16_t> tex_,
               WritingModeType writingMode_,
               Point<float> glyphOffset_,
               bool isSDF_,
               Point<float> pixelOffsetTL_,
               Point<float> pixelOffsetBR_,
               Point<float> minFontScale_,
               size_t sectionIndex_ = 0)
        : tl(tl_),
          tr(tr_),
          bl(bl_),
          br(br_),
          tex(tex_),
          pixelOffsetTL(pixelOffsetTL_),
          pixelOffsetBR(pixelOffsetBR_),
          glyphOffset(glyphOffset_),
          writingMode(writingMode_),
          isSDF(isSDF_),
          sectionIndex(sectionIndex_),
          minFontScale(minFontScale_) {}

    Point<float> tl;
    Point<float> tr;
    Point<float> bl;
    Point<float> br;
    Rect<uint16_t> tex;
    Point<float> pixelOffsetTL;
    Point<float> pixelOffsetBR;
    Point<float> glyphOffset;
    WritingModeType writingMode;
    bool isSDF;
    size_t sectionIndex;
    Point<float> minFontScale;
};

using SymbolQuads = std::vector<SymbolQuad>;

SymbolQuads getIconQuads(const PositionedIcon& shapedIcon,
                         float iconRotate,
                         SymbolContent iconType,
                         bool hasIconTextFit);

SymbolQuads getGlyphQuads(const Shaping& shapedText,
                          std::array<float, 2> textOffset,
                          const style::SymbolLayoutProperties::Evaluated&,
                          style::SymbolPlacementType placement,
                          const ImageMap& imageMap,
                          bool allowVerticalPlacement);

} // namespace mbgl
