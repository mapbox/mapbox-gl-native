#include "../fixtures/util.hpp"

#include <mbgl/util/assert.hpp>

using namespace mbgl;

TEST(Assert, Always) {
    EXPECT_DEATH(assert_always(true == false), "failed assertion `true == false'");
}
