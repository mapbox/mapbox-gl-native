#include <mbgl/test/util.hpp>

#include <mbgl/style/rotation.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>

using namespace mbgl;
using namespace style;

Rotation createRotation(double angle) {
    return Rotation(angle);
}

TEST(Rotation, Calculations) {
    Rotation rot(42);
    EXPECT_EQ(rot, Rotation(42 + 360));
    Rotation rot2(-42);
    EXPECT_EQ(rot2, Rotation(360 - 42));

    mbgl::util::Interpolator<mbgl::style::Rotation> i;
    EXPECT_NEAR(i(rot, rot2, 0.5).getAngle(), 0.0, 0.00001);
}
