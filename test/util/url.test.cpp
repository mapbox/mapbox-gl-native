#include <mbgl/test/util.hpp>

#include <mbgl/util/url.hpp>

#include <memory>

using namespace mbgl::util;

TEST(URL, isURL) {
    EXPECT_TRUE(isURL("mapbox://foo"));
    EXPECT_TRUE(isURL("mapbox://"));
    EXPECT_TRUE(isURL("mapbox-test-scheme://foo"));
    EXPECT_TRUE(isURL("mapbox+style://foo"));
    EXPECT_TRUE(isURL("mapbox-2.0://foo"));
    EXPECT_TRUE(isURL("mapbox99://foo"));

    EXPECT_FALSE(isURL("mapbox:/"));
    EXPECT_FALSE(isURL(" mapbox://"));
    EXPECT_FALSE(isURL("://"));
    EXPECT_FALSE(isURL("mapbox"));
    EXPECT_FALSE(isURL("mapbox:foo"));
    EXPECT_FALSE(isURL("mapbox:/foo"));
    EXPECT_FALSE(isURL("test/mapbox://foo"));
    EXPECT_FALSE(isURL("123://foo"));
}
