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
        std::shared_ptr<const SpriteImage>(),
        1.0f
    };
    PositionedIcon shapedIcon(image, -5.0, 6.0, -7.0, 8.0);
    GeometryCoordinates line;
    Shaping shapedText;

    SymbolQuads quads =
        getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

    ASSERT_EQ(quads.size(), 1u);
    ASSERT_EQ(quads[0].anchorPoint.x, 2);
    ASSERT_EQ(quads[0].anchorPoint.y, 3);
    ASSERT_EQ(quads[0].tl.x, -8);
    ASSERT_EQ(quads[0].tl.y, -6);
    ASSERT_EQ(quads[0].tr.x, 7);
    ASSERT_EQ(quads[0].tr.y, -6);
    ASSERT_EQ(quads[0].bl.x, -8);
    ASSERT_EQ(quads[0].bl.y, 5);
    ASSERT_EQ(quads[0].br.x, 7);
    ASSERT_EQ(quads[0].br.y, 5);
    ASSERT_EQ(quads[0].anchorAngle, 0.0f);
    ASSERT_EQ(quads[0].glyphAngle, 0.0f);
    ASSERT_EQ(quads[0].minScale, 0.5f);
}

TEST(getIconQuads, style) {
    Anchor anchor(0.0, 0.0, 0.0, 0.5f, 0);
    SpriteAtlasElement image = {
        Rect<uint16_t>( 0, 0, 20, 20 ),
        std::shared_ptr<const SpriteImage>(),
        1.0f
    };
    PositionedIcon shapedIcon(image, -10.0, 10.0, -10.0, 10.0);
    GeometryCoordinates line;
    Shaping shapedText;
    shapedText.top = -10.0f;
    shapedText.bottom = 30.0f;
    shapedText.left = -60.0f;
    shapedText.right = 20.0f;
    shapedText.positionedGlyphs.emplace_back(PositionedGlyph(32, 0.0f, 0.0f));

    // none
    {
        SymbolLayoutProperties::Evaluated layout;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads.size(), 1u);
        ASSERT_EQ(quads[0].anchorPoint.x, 0);
        ASSERT_EQ(quads[0].anchorPoint.y, 0);
        ASSERT_EQ(quads[0].tl.x, -11);
        ASSERT_EQ(quads[0].tl.y, -11);
        ASSERT_EQ(quads[0].tr.x, 9);
        ASSERT_EQ(quads[0].tr.y, -11);
        ASSERT_EQ(quads[0].bl.x, -11);
        ASSERT_EQ(quads[0].bl.y, 9);
        ASSERT_EQ(quads[0].br.x, 9);
        ASSERT_EQ(quads[0].br.y, 9);
        ASSERT_EQ(quads[0].anchorAngle, 0.0f);
        ASSERT_EQ(quads[0].glyphAngle, 0.0f);
        ASSERT_EQ(quads[0].minScale, 0.5f);
    }

    // width
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -60);
        ASSERT_EQ(quads[0].tl.y, 0);
        ASSERT_EQ(quads[0].tr.x, 20);
        ASSERT_EQ(quads[0].tr.y, 0);
        ASSERT_EQ(quads[0].bl.x, -60);
        ASSERT_EQ(quads[0].bl.y, 20);
        ASSERT_EQ(quads[0].br.x, 20);
        ASSERT_EQ(quads[0].br.y, 20);
    }

    // width x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Width;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -30);
        ASSERT_EQ(quads[0].tl.y, -5);
        ASSERT_EQ(quads[0].tr.x, 10);
        ASSERT_EQ(quads[0].tr.y, -5);
        ASSERT_EQ(quads[0].bl.x, -30);
        ASSERT_EQ(quads[0].bl.y, 15);
        ASSERT_EQ(quads[0].br.x, 10);
        ASSERT_EQ(quads[0].br.y, 15);
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
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -40);
        ASSERT_EQ(quads[0].tl.y, -10);
        ASSERT_EQ(quads[0].tr.x, 20);
        ASSERT_EQ(quads[0].tr.y, -10);
        ASSERT_EQ(quads[0].bl.x, -40);
        ASSERT_EQ(quads[0].bl.y, 20);
        ASSERT_EQ(quads[0].br.x, 20);
        ASSERT_EQ(quads[0].br.y, 20);
    }

    // height
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -30);
        ASSERT_EQ(quads[0].tl.y, -10);
        ASSERT_EQ(quads[0].tr.x, -10);
        ASSERT_EQ(quads[0].tr.y, -10);
        ASSERT_EQ(quads[0].bl.x, -30);
        ASSERT_EQ(quads[0].bl.y, 30);
        ASSERT_EQ(quads[0].br.x, -10);
        ASSERT_EQ(quads[0].br.y, 30);
    }

    // height x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Height;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -20);
        ASSERT_EQ(quads[0].tl.y, -5);
        ASSERT_EQ(quads[0].tr.x, 0);
        ASSERT_EQ(quads[0].tr.y, -5);
        ASSERT_EQ(quads[0].bl.x, -20);
        ASSERT_EQ(quads[0].bl.y, 15);
        ASSERT_EQ(quads[0].br.x, 0);
        ASSERT_EQ(quads[0].br.y, 15);
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
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -30);
        ASSERT_EQ(quads[0].tl.y, -10);
        ASSERT_EQ(quads[0].tr.x, 10);
        ASSERT_EQ(quads[0].tr.y, -10);
        ASSERT_EQ(quads[0].bl.x, -30);
        ASSERT_EQ(quads[0].bl.y, 20);
        ASSERT_EQ(quads[0].br.x, 10);
        ASSERT_EQ(quads[0].br.y, 20);
    }

    // both
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 24.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -60);
        ASSERT_EQ(quads[0].tl.y, -10);
        ASSERT_EQ(quads[0].tr.x, 20);
        ASSERT_EQ(quads[0].tr.y, -10);
        ASSERT_EQ(quads[0].bl.x, -60);
        ASSERT_EQ(quads[0].bl.y, 30);
        ASSERT_EQ(quads[0].br.x, 20);
        ASSERT_EQ(quads[0].br.y, 30);
    }

    // both x textSize
    {
        SymbolLayoutProperties::Evaluated layout;
        layout.get<TextSize>() = 12.0f;
        layout.get<IconTextFit>() = IconTextFitType::Both;
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -30);
        ASSERT_EQ(quads[0].tl.y, -5);
        ASSERT_EQ(quads[0].tr.x, 10);
        ASSERT_EQ(quads[0].tr.y, -5);
        ASSERT_EQ(quads[0].bl.x, -30);
        ASSERT_EQ(quads[0].bl.y, 15);
        ASSERT_EQ(quads[0].br.x, 10);
        ASSERT_EQ(quads[0].br.y, 15);
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
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -40);
        ASSERT_EQ(quads[0].tl.y, -10);
        ASSERT_EQ(quads[0].tr.x, 20);
        ASSERT_EQ(quads[0].tr.y, -10);
        ASSERT_EQ(quads[0].bl.x, -40);
        ASSERT_EQ(quads[0].bl.y, 20);
        ASSERT_EQ(quads[0].br.x, 20);
        ASSERT_EQ(quads[0].br.y, 20);
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
        SymbolQuads quads =
            getIconQuads(anchor, shapedIcon, line, layout, SymbolPlacementType::Point, shapedText);

        ASSERT_EQ(quads[0].tl.x, -45);
        ASSERT_EQ(quads[0].tl.y, -5);
        ASSERT_EQ(quads[0].tr.x, 15);
        ASSERT_EQ(quads[0].tr.y, -5);
        ASSERT_EQ(quads[0].bl.x, -45);
        ASSERT_EQ(quads[0].bl.y, 25);
        ASSERT_EQ(quads[0].br.x, 15);
        ASSERT_EQ(quads[0].br.y, 25);
    }
}

