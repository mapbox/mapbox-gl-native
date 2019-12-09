#include <mbgl/geometry/anchor.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/text/shaping.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(getIconQuads, normal) {
    SymbolLayoutProperties::Evaluated layout;
    Anchor anchor(2.0, 3.0, 0.0, 0);
    ImagePosition image = {
        mapbox::Bin(-1, 15, 11, 0, 0, 0, 0),
        style::Image::Impl("test", PremultipliedImage({1,1}), 1.0)
    };

    auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -6.5f, -4.5f }}, SymbolAnchorType::Center, 0);

    GeometryCoordinates line;

    SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

    EXPECT_EQ(quad.tl.x, -14);
    EXPECT_EQ(quad.tl.y, -10);
    EXPECT_EQ(quad.tr.x, 1);
    EXPECT_EQ(quad.tr.y, -10);
    EXPECT_EQ(quad.bl.x, -14);
    EXPECT_EQ(quad.bl.y, 1);
    EXPECT_EQ(quad.br.x, 1);
    EXPECT_EQ(quad.br.y, 1);
}

TEST(getIconQuads, style) {
    Anchor anchor(0.0, 0.0, 0.0, 0);
    const ImagePosition image = {mapbox::Bin(-1, 20, 20, 0, 0, 0, 0),
                                 style::Image::Impl("test", PremultipliedImage({1, 1}), 1.0)};

    GeometryCoordinates line;
    Shaping shapedText;
    shapedText.top = -10.0f;
    shapedText.bottom = 30.0f;
    shapedText.left = -60.0f;
    shapedText.right = 20.0f;
    // shapedText.positionedGlyphs.emplace_back(PositionedGlyph(32, 0.0f, 0.0f, false, 0, 1.0));
    shapedText.positionedLines.emplace_back();
    shapedText.positionedLines.back().positionedGlyphs.emplace_back(
        PositionedGlyph(32, 0.0f, 0.0f, false, 0, 1.0, /*texRect*/ {}, /*metrics*/ {}, /*imageID*/ nullopt));

    // none
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);

        EXPECT_FLOAT_EQ(-18.5f, shapedIcon.top());
        EXPECT_FLOAT_EQ(-0.5f, shapedIcon.right());
        EXPECT_FLOAT_EQ(-0.5f, shapedIcon.bottom());
        EXPECT_FLOAT_EQ(-18.5f, shapedIcon.left());

        SymbolLayoutProperties::Evaluated layout;
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -19.5);
        EXPECT_FLOAT_EQ(quad.tl.y, -19.5);
        EXPECT_FLOAT_EQ(quad.tr.x, 0.5);
        EXPECT_FLOAT_EQ(quad.tr.y, -19.5);
        EXPECT_FLOAT_EQ(quad.bl.x, -19.5);
        EXPECT_FLOAT_EQ(quad.bl.y, 0.5);
        EXPECT_FLOAT_EQ(quad.br.x, 0.5);
        EXPECT_FLOAT_EQ(quad.br.y, 0.5);
    }

    // width
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Width, {{0, 0, 0, 0}}, {{0, 0}}, 24.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -64.4444427);
        EXPECT_FLOAT_EQ(quad.tl.y, 0);
        EXPECT_FLOAT_EQ(quad.tr.x, 24.4444427);
        EXPECT_FLOAT_EQ(quad.tr.y, 0);
        EXPECT_FLOAT_EQ(quad.bl.x, -64.4444427);
        EXPECT_FLOAT_EQ(quad.bl.y, 20);
        EXPECT_FLOAT_EQ(quad.br.x, 24.4444427);
        EXPECT_FLOAT_EQ(quad.br.y, 20);
    }

    // width x textSize
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Width, {{0, 0, 0, 0}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -32.2222214);
        EXPECT_FLOAT_EQ(quad.tl.y, -5);
        EXPECT_FLOAT_EQ(quad.tr.x, 12.2222214);
        EXPECT_FLOAT_EQ(quad.tr.y, -5);
        EXPECT_FLOAT_EQ(quad.bl.x, -32.2222214);
        EXPECT_FLOAT_EQ(quad.bl.y, 15);
        EXPECT_FLOAT_EQ(quad.br.x, 12.2222214);
        EXPECT_FLOAT_EQ(quad.br.y, 15);
    }

    // width x textSize + padding
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Width, {{5, 10, 5, 10}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -43.3333321);
        EXPECT_FLOAT_EQ(quad.tl.y, -5);
        EXPECT_FLOAT_EQ(quad.tr.x, 23.3333321);
        EXPECT_FLOAT_EQ(quad.tr.y, -5);
        EXPECT_FLOAT_EQ(quad.bl.x, -43.3333321);
        EXPECT_FLOAT_EQ(quad.bl.y, 15);
        EXPECT_FLOAT_EQ(quad.br.x, 23.3333321);
        EXPECT_FLOAT_EQ(quad.br.y, 15);
    }

    // height
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Height, {{0, 0, 0, 0}}, {{0, 0}}, 24.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -30);
        EXPECT_FLOAT_EQ(quad.tl.y, -12.2222214);
        EXPECT_FLOAT_EQ(quad.tr.x, -10);
        EXPECT_FLOAT_EQ(quad.tr.y, -12.2222214);
        EXPECT_FLOAT_EQ(quad.bl.x, -30);
        EXPECT_FLOAT_EQ(quad.bl.y, 32.2222214);
        EXPECT_FLOAT_EQ(quad.br.x, -10);
        EXPECT_FLOAT_EQ(quad.br.y, 32.2222214);
    }

    // height x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Height, {{0, 0, 0, 0}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -20);
        EXPECT_FLOAT_EQ(quad.tl.y, -6.11111069);
        EXPECT_FLOAT_EQ(quad.tr.x, 0);
        EXPECT_FLOAT_EQ(quad.tr.y, -6.11111069);
        EXPECT_FLOAT_EQ(quad.bl.x, -20);
        EXPECT_FLOAT_EQ(quad.bl.y, 16.1111107);
        EXPECT_FLOAT_EQ(quad.br.x, 0);
        EXPECT_FLOAT_EQ(quad.br.y, 16.1111107);
    }

    // height x textSize + padding
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Height, {{5, 10, 5, 20}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -20);
        EXPECT_FLOAT_EQ(quad.tl.y, -11.666666);
        EXPECT_FLOAT_EQ(quad.tr.x, 0);
        EXPECT_FLOAT_EQ(quad.tr.y, -11.666666);
        EXPECT_FLOAT_EQ(quad.bl.x, -20);
        EXPECT_FLOAT_EQ(quad.bl.y, 21.666666);
        EXPECT_FLOAT_EQ(quad.br.x, 0);
        EXPECT_FLOAT_EQ(quad.br.y, 21.666666);
    }

    // both
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Both, {{0, 0, 0, 0}}, {{0, 0}}, 24.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -64.4444427);
        EXPECT_FLOAT_EQ(quad.tl.y, -12.2222214);
        EXPECT_FLOAT_EQ(quad.tr.x, 24.4444427);
        EXPECT_FLOAT_EQ(quad.tr.y, -12.2222214);
        EXPECT_FLOAT_EQ(quad.bl.x, -64.4444427);
        EXPECT_FLOAT_EQ(quad.bl.y, 32.2222214);
        EXPECT_FLOAT_EQ(quad.br.x, 24.4444427);
        EXPECT_FLOAT_EQ(quad.br.y, 32.2222214);
    }

    // both x textSize
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Both, {{0, 0, 0, 0}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -32.2222214);
        EXPECT_FLOAT_EQ(quad.tl.y, -6.11111069);
        EXPECT_FLOAT_EQ(quad.tr.x, 12.2222214);
        EXPECT_FLOAT_EQ(quad.tr.y, -6.11111069);
        EXPECT_FLOAT_EQ(quad.bl.x, -32.2222214);
        EXPECT_FLOAT_EQ(quad.bl.y, 16.1111107);
        EXPECT_FLOAT_EQ(quad.br.x, 12.2222214);
        EXPECT_FLOAT_EQ(quad.br.y, 16.1111107);
    }

    // both x textSize + padding
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Both, {{5, 10, 5, 10}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -43.3333321);
        EXPECT_FLOAT_EQ(quad.tl.y, -11.666666);
        EXPECT_FLOAT_EQ(quad.tr.x, 23.3333321);
        EXPECT_FLOAT_EQ(quad.tr.y, -11.666666);
        EXPECT_FLOAT_EQ(quad.bl.x, -43.3333321);
        EXPECT_FLOAT_EQ(quad.bl.y, 21.666666);
        EXPECT_FLOAT_EQ(quad.br.x, 23.3333321);
        EXPECT_FLOAT_EQ(quad.br.y, 21.666666);
    }

    // both x textSize + padding t/r/b/l
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{-9.5f, -9.5f}}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(shapedText, IconTextFitType::Both, {{0, 5, 10, 15}}, {{0, 0}}, 12.0f / 24.0f);
        SymbolQuad quad = getIconQuad(shapedIcon, WritingModeType::Horizontal, SymbolContent::IconRGBA);

        EXPECT_FLOAT_EQ(quad.tl.x, -48.3333321);
        EXPECT_FLOAT_EQ(quad.tl.y, -6.66666603);
        EXPECT_FLOAT_EQ(quad.tr.x, 18.3333321);
        EXPECT_FLOAT_EQ(quad.tr.y, -6.66666603);
        EXPECT_FLOAT_EQ(quad.bl.x, -48.3333321);
        EXPECT_FLOAT_EQ(quad.bl.y, 26.666666);
        EXPECT_FLOAT_EQ(quad.br.x, 18.3333321);
        EXPECT_FLOAT_EQ(quad.br.y, 26.666666);
    }
}
