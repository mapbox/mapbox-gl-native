#include <mbgl/test/util.hpp>

#include <mbgl/style/types.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/position.hpp>

using namespace mbgl;
using namespace style;

auto ARR_EQ = [](std::array<float, 3> got, std::array<float, 3> expected) {
    for (int i = 0; i < 3; i++) {
        EXPECT_NEAR(got[i], expected[i], 0.00001);
    }
};

auto ARR_NE = [](std::array<float, 3> got, std::array<float, 3> expected) {
    short eq = 0;
    for (int i = 0; i < 3; i++) {
        if (got[i] == expected[i]) {
            eq++;
        }
    }
    EXPECT_NE(eq, 3);
};

TEST(Position, Calculations) {
    std::array<float, 3> spherical{{ 2, 10, 270 }};

    Position position(spherical);

    ARR_NE(position.get(LightAnchorType::Map, 0), spherical);
    ARR_EQ(position.getSpherical(), spherical);

    ARR_EQ(position.get(LightAnchorType::Map, 0), {{ 0.34729638695716858, -1.9696154594421387, 2.384976127700611e-08 }});

    ARR_NE(position.get(LightAnchorType::Map, 90), position.get(LightAnchorType::Viewport, 90));
    ARR_EQ(position.get(LightAnchorType::Map, 180), position.get(LightAnchorType::Map, 270));

    ARR_NE(Position{{{ 2, 30, 10 }}}.getSpherical(), Position{{{ 2, 30, 370 }}}.getSpherical());
    ARR_EQ(Position{{{ 2, 30, 10 }}}.get(LightAnchorType::Map, 0), Position{{{ 2, 30, 370 }}}.get(LightAnchorType::Map, 0));

    position.set({{ 1, 80, 270 }});

    ARR_NE(position.getSpherical(), spherical);
    ARR_NE(position.get(LightAnchorType::Map, 0), {{ 0.34729638695716858, -1.9696154594421387, 2.384976127700611e-08 }});
    ARR_EQ(position.get(LightAnchorType::Map, 0), {{ 0.98480772972106934, -0.17364829778671265, 1.1924880638503055e-08 }});
}
