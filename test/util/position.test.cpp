#include <mbgl/test/util.hpp>

#include <mbgl/style/types.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/util/constants.hpp>

using namespace mbgl;
using namespace style;

void expectArrayEQ(std::array<float, 3> got, std::array<float, 3> expected) {
    for (int i = 0; i < 3; i++) {
        EXPECT_NEAR(got[i], expected[i], 0.00001);
    }
};

void expectArrayNE(std::array<float, 3> got, std::array<float, 3> expected) {
    short eq = 0;
    for (int i = 0; i < 3; i++) {
        if (got[i] == expected[i]) {
            eq++;
        }
    }
    EXPECT_NE(eq, 3);
};

Position createPosition(std::array<float, 3> pos) {
    return Position(pos);
}

TEST(Position, Calculations) {
    std::array<float, 3> spherical{{ 2, 10, 270 }};

    Position position(spherical);

    expectArrayNE(position.getCartesian(), spherical);
    expectArrayEQ(position.getSpherical(), spherical);

    expectArrayEQ(position.getCartesian(), {{ 0.34729638695716858, -1.9696154594421387, 2.384976127700611e-08 }});

    expectArrayNE(createPosition({{ 2, 30, 10 }}).getSpherical(), createPosition({{ 2, 30, 370 }}).getSpherical());
    expectArrayEQ(createPosition({{ 2, 30, 10 }}).getCartesian(), createPosition({{ 2, 30, 370 }}).getCartesian());

    std::array<float, 3> newSpherical = {{ 1, 80, 270 }};
    position.set(newSpherical);

    expectArrayNE(position.getSpherical(), spherical);
    expectArrayNE(position.getCartesian(), {{ 0.34729638695716858, -1.9696154594421387, 2.384976127700611e-08 }});
    expectArrayEQ(position.getCartesian(), {{ 0.98480772972106934, -0.17364829778671265, 1.1924880638503055e-08 }});
}
