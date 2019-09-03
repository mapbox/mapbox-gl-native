#include <mbgl/geometry/anchor.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/text/quads.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(getIconQuads, normal) {
    SymbolLayoutProperties::Evaluated layout;
    Anchor anchor(2.0, 3.0, 0.0, 0.5f, 0);
    ImagePosition image = {
        mapbox::Bin(-1, 15, 11, 0, 0, 0, 0),
        style::Image::Impl("test", PremultipliedImage({1,1}), 1.0)
    };

    auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -6.5f, -4.5f }}, SymbolAnchorType::Center, 0);

    GeometryCoordinates line;

    SymbolQuad quad =
        getIconQuad(shapedIcon, WritingModeType::Horizontal);

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
    Anchor anchor(0.0, 0.0, 0.0, 0.5f, 0);
    ImagePosition image = {
        mapbox::Bin(-1, 20, 20, 0, 0, 0, 0),
        style::Image::Impl("test", PremultipliedImage({1,1}), 1.0)
    };

    GeometryCoordinates line;
    Shaping shapedText;
    shapedText.top = -10.0f;
    shapedText.bottom = 30.0f;
    shapedText.left = -60.0f;
    shapedText.right = 20.0f;
    shapedText.positionedGlyphs.emplace_back(PositionedGlyph(32, 0.0f, 0.0f, false, 0, 1.0));

    // none
    {
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        SymbolLayoutProperties::Evaluated layout;
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -19.5);
        EXPECT_EQ(quad.tl.y, -19.5);
        EXPECT_EQ(quad.tr.x, 0.5);
        EXPECT_EQ(quad.tr.y, -19.5);
        EXPECT_EQ(quad.bl.x, -19.5);
        EXPECT_EQ(quad.bl.y, 0.5);
        EXPECT_EQ(quad.br.x, 0.5);
        EXPECT_EQ(quad.br.y, 0.5);
    }

    // width
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 24.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -61);
        EXPECT_EQ(quad.tl.y, 0);
        EXPECT_EQ(quad.tr.x, 21);
        EXPECT_EQ(quad.tr.y, 0);
        EXPECT_EQ(quad.bl.x, -61);
        EXPECT_EQ(quad.bl.y, 20);
        EXPECT_EQ(quad.br.x, 21);
        EXPECT_EQ(quad.br.y, 20);
    }

    // width x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -31);
        EXPECT_EQ(quad.tl.y, -5);
        EXPECT_EQ(quad.tr.x, 11);
        EXPECT_EQ(quad.tr.y, -5);
        EXPECT_EQ(quad.bl.x, -31);
        EXPECT_EQ(quad.bl.y, 15);
        EXPECT_EQ(quad.br.x, 11);
        EXPECT_EQ(quad.br.y, 15);
    }

    // width x textSize + padding
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        layout.get<IconTextFitPadding>()[0] = 5.0f;
        layout.get<IconTextFitPadding>()[1] = 10.0f;
        layout.get<IconTextFitPadding>()[2] = 5.0f;
        layout.get<IconTextFitPadding>()[3] = 10.0f;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -41);
        EXPECT_EQ(quad.tl.y, -10);
        EXPECT_EQ(quad.tr.x, 21);
        EXPECT_EQ(quad.tr.y, -10);
        EXPECT_EQ(quad.bl.x, -41);
        EXPECT_EQ(quad.bl.y, 20);
        EXPECT_EQ(quad.br.x, 21);
        EXPECT_EQ(quad.br.y, 20);
    }

    // height
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 24.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -30);
        EXPECT_EQ(quad.tl.y, -11);
        EXPECT_EQ(quad.tr.x, -10);
        EXPECT_EQ(quad.tr.y, -11);
        EXPECT_EQ(quad.bl.x, -30);
        EXPECT_EQ(quad.bl.y, 31);
        EXPECT_EQ(quad.br.x, -10);
        EXPECT_EQ(quad.br.y, 31);
    }

    // height x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -20);
        EXPECT_EQ(quad.tl.y, -6);
        EXPECT_EQ(quad.tr.x, 0);
        EXPECT_EQ(quad.tr.y, -6);
        EXPECT_EQ(quad.bl.x, -20);
        EXPECT_EQ(quad.bl.y, 16);
        EXPECT_EQ(quad.br.x, 0);
        EXPECT_EQ(quad.br.y, 16);
    }

    // height x textSize + padding
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        layout.get<IconTextFitPadding>()[0] = 5.0f;
        layout.get<IconTextFitPadding>()[1] = 10.0f;
        layout.get<IconTextFitPadding>()[2] = 5.0f;
        layout.get<IconTextFitPadding>()[3] = 10.0f;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -30);
        EXPECT_EQ(quad.tl.y, -11);
        EXPECT_EQ(quad.tr.x, 10);
        EXPECT_EQ(quad.tr.y, -11);
        EXPECT_EQ(quad.bl.x, -30);
        EXPECT_EQ(quad.bl.y, 21);
        EXPECT_EQ(quad.br.x, 10);
        EXPECT_EQ(quad.br.y, 21);
    }

    // both
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 24.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -61);
        EXPECT_EQ(quad.tl.y, -11);
        EXPECT_EQ(quad.tr.x, 21);
        EXPECT_EQ(quad.tr.y, -11);
        EXPECT_EQ(quad.bl.x, -61);
        EXPECT_EQ(quad.bl.y, 31);
        EXPECT_EQ(quad.br.x, 21);
        EXPECT_EQ(quad.br.y, 31);
    }

    // both x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -31);
        EXPECT_EQ(quad.tl.y, -6);
        EXPECT_EQ(quad.tr.x, 11);
        EXPECT_EQ(quad.tr.y, -6);
        EXPECT_EQ(quad.bl.x, -31);
        EXPECT_EQ(quad.bl.y, 16);
        EXPECT_EQ(quad.br.x, 11);
        EXPECT_EQ(quad.br.y, 16);
    }

    // both x textSize + padding
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        layout.get<IconTextFitPadding>()[0] = 5.0f;
        layout.get<IconTextFitPadding>()[1] = 10.0f;
        layout.get<IconTextFitPadding>()[2] = 5.0f;
        layout.get<IconTextFitPadding>()[3] = 10.0f;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -41);
        EXPECT_EQ(quad.tl.y, -11);
        EXPECT_EQ(quad.tr.x, 21);
        EXPECT_EQ(quad.tr.y, -11);
        EXPECT_EQ(quad.bl.x, -41);
        EXPECT_EQ(quad.bl.y, 21);
        EXPECT_EQ(quad.br.x, 21);
        EXPECT_EQ(quad.br.y, 21);
    }

    // both x textSize + padding t/r/b/l
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        layout.get<IconTextFitPadding>()[0] = 0.0f;
        layout.get<IconTextFitPadding>()[1] = 5.0f;
        layout.get<IconTextFitPadding>()[2] = 10.0f;
        layout.get<IconTextFitPadding>()[3] = 15.0f;
        auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, SymbolAnchorType::Center, 0);
        shapedIcon.fitIconToText(layout, shapedText, 12.0f);
        SymbolQuad quad =
            getIconQuad(shapedIcon, WritingModeType::Horizontal);

        EXPECT_EQ(quad.tl.x, -46);
        EXPECT_EQ(quad.tl.y, -6);
        EXPECT_EQ(quad.tr.x, 16);
        EXPECT_EQ(quad.tr.y, -6);
        EXPECT_EQ(quad.bl.x, -46);
        EXPECT_EQ(quad.bl.y, 26);
        EXPECT_EQ(quad.br.x, 16);
        EXPECT_EQ(quad.br.y, 26);
    }
}

