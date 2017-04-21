#include <mbgl/geometry/anchor.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
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
    SpriteAtlasElement image = {
        Rect<uint16_t>( 0, 0, 15, 11 ),
        style::Image(PremultipliedImage({1,1}), 1.0),
        { 0, 0 },
        1.0f
    };

    auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -6.5f, -4.5f }}, 0);
    ASSERT_TRUE(shapedIcon);

    GeometryCoordinates line;
    Shaping shapedText;

    SymbolQuad quad =
        getIconQuad(anchor, *shapedIcon, line, layout, 16.0f, SymbolPlacementType::Point, shapedText);

    ASSERT_EQ(quad.anchorPoint.x, 2);
    ASSERT_EQ(quad.anchorPoint.y, 3);
    ASSERT_EQ(quad.tl.x, -8);
    ASSERT_EQ(quad.tl.y, -6);
    ASSERT_EQ(quad.tr.x, 7);
    ASSERT_EQ(quad.tr.y, -6);
    ASSERT_EQ(quad.bl.x, -8);
    ASSERT_EQ(quad.bl.y, 5);
    ASSERT_EQ(quad.br.x, 7);
    ASSERT_EQ(quad.br.y, 5);
    ASSERT_EQ(quad.anchorAngle, 0.0f);
    ASSERT_EQ(quad.glyphAngle, 0.0f);
    ASSERT_EQ(quad.minScale, 0.5f);
}

TEST(getIconQuads, style) {
    Anchor anchor(0.0, 0.0, 0.0, 0.5f, 0);
    SpriteAtlasElement image = {
        Rect<uint16_t>( 0, 0, 20, 20 ),
        style::Image(PremultipliedImage({1,1}), 1.0),
        { 0, 0 },
        1.0f
    };

    auto shapedIcon = PositionedIcon::shapeIcon(image, {{ -9.5f, -9.5f }}, 0);
    ASSERT_TRUE(shapedIcon);

    GeometryCoordinates line;
    Shaping shapedText;
    shapedText.top = -10.0f;
    shapedText.bottom = 30.0f;
    shapedText.left = -60.0f;
    shapedText.right = 20.0f;
    shapedText.positionedGlyphs.emplace_back(PositionedGlyph(32, 0.0f, 0.0f, 0));

    // none
    {
        SymbolLayoutProperties::Evaluated layout;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.anchorPoint.x, 0);
        ASSERT_EQ(quad.anchorPoint.y, 0);
        ASSERT_EQ(quad.tl.x, -11);
        ASSERT_EQ(quad.tl.y, -11);
        ASSERT_EQ(quad.tr.x, 9);
        ASSERT_EQ(quad.tr.y, -11);
        ASSERT_EQ(quad.bl.x, -11);
        ASSERT_EQ(quad.bl.y, 9);
        ASSERT_EQ(quad.br.x, 9);
        ASSERT_EQ(quad.br.y, 9);
        ASSERT_EQ(quad.anchorAngle, 0.0f);
        ASSERT_EQ(quad.glyphAngle, 0.0f);
        ASSERT_EQ(quad.minScale, 0.5f);
    }

    // width
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 24.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -60);
        ASSERT_EQ(quad.tl.y, 0);
        ASSERT_EQ(quad.tr.x, 20);
        ASSERT_EQ(quad.tr.y, 0);
        ASSERT_EQ(quad.bl.x, -60);
        ASSERT_EQ(quad.bl.y, 20);
        ASSERT_EQ(quad.br.x, 20);
        ASSERT_EQ(quad.br.y, 20);
    }

    // width x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -30);
        ASSERT_EQ(quad.tl.y, -5);
        ASSERT_EQ(quad.tr.x, 10);
        ASSERT_EQ(quad.tr.y, -5);
        ASSERT_EQ(quad.bl.x, -30);
        ASSERT_EQ(quad.bl.y, 15);
        ASSERT_EQ(quad.br.x, 10);
        ASSERT_EQ(quad.br.y, 15);
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
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -40);
        ASSERT_EQ(quad.tl.y, -10);
        ASSERT_EQ(quad.tr.x, 20);
        ASSERT_EQ(quad.tr.y, -10);
        ASSERT_EQ(quad.bl.x, -40);
        ASSERT_EQ(quad.bl.y, 20);
        ASSERT_EQ(quad.br.x, 20);
        ASSERT_EQ(quad.br.y, 20);
    }

    // height
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 24.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -30);
        ASSERT_EQ(quad.tl.y, -10);
        ASSERT_EQ(quad.tr.x, -10);
        ASSERT_EQ(quad.tr.y, -10);
        ASSERT_EQ(quad.bl.x, -30);
        ASSERT_EQ(quad.bl.y, 30);
        ASSERT_EQ(quad.br.x, -10);
        ASSERT_EQ(quad.br.y, 30);
    }

    // height x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -20);
        ASSERT_EQ(quad.tl.y, -5);
        ASSERT_EQ(quad.tr.x, 0);
        ASSERT_EQ(quad.tr.y, -5);
        ASSERT_EQ(quad.bl.x, -20);
        ASSERT_EQ(quad.bl.y, 15);
        ASSERT_EQ(quad.br.x, 0);
        ASSERT_EQ(quad.br.y, 15);
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
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -30);
        ASSERT_EQ(quad.tl.y, -10);
        ASSERT_EQ(quad.tr.x, 10);
        ASSERT_EQ(quad.tr.y, -10);
        ASSERT_EQ(quad.bl.x, -30);
        ASSERT_EQ(quad.bl.y, 20);
        ASSERT_EQ(quad.br.x, 10);
        ASSERT_EQ(quad.br.y, 20);
    }

    // both
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 24.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -60);
        ASSERT_EQ(quad.tl.y, -10);
        ASSERT_EQ(quad.tr.x, 20);
        ASSERT_EQ(quad.tr.y, -10);
        ASSERT_EQ(quad.bl.x, -60);
        ASSERT_EQ(quad.bl.y, 30);
        ASSERT_EQ(quad.br.x, 20);
        ASSERT_EQ(quad.br.y, 30);
    }

    // both x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -30);
        ASSERT_EQ(quad.tl.y, -5);
        ASSERT_EQ(quad.tr.x, 10);
        ASSERT_EQ(quad.tr.y, -5);
        ASSERT_EQ(quad.bl.x, -30);
        ASSERT_EQ(quad.bl.y, 15);
        ASSERT_EQ(quad.br.x, 10);
        ASSERT_EQ(quad.br.y, 15);
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
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -40);
        ASSERT_EQ(quad.tl.y, -10);
        ASSERT_EQ(quad.tr.x, 20);
        ASSERT_EQ(quad.tr.y, -10);
        ASSERT_EQ(quad.bl.x, -40);
        ASSERT_EQ(quad.bl.y, 20);
        ASSERT_EQ(quad.br.x, 20);
        ASSERT_EQ(quad.br.y, 20);
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
        SymbolQuad quad =
            getIconQuad(anchor, *shapedIcon, line, layout, 12.0f, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quad.tl.x, -45);
        ASSERT_EQ(quad.tl.y, -5);
        ASSERT_EQ(quad.tr.x, 15);
        ASSERT_EQ(quad.tr.y, -5);
        ASSERT_EQ(quad.bl.x, -45);
        ASSERT_EQ(quad.bl.y, 25);
        ASSERT_EQ(quad.br.x, 15);
        ASSERT_EQ(quad.br.y, 25);
    }
}

