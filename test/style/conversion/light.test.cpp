#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/chrono.hpp>

#include <array>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(StyleConversion, Light) {
    Error error;

    auto parseLight = [&](const std::string& src) {
        return convertJSON<Light>(src, error);
    };

    {
        auto light = parseLight("{}");
        ASSERT_TRUE((bool) light);
    }

    {
        auto light = parseLight(R"({"color":{"stops":[[14,"blue"],[16,"red"]]},"intensity":0.3,"position":[3,90,90]})");
        ASSERT_TRUE((bool) light);

        ASSERT_TRUE(light->getAnchor().isUndefined());
        ASSERT_FALSE(light->getAnchor().isConstant());
        ASSERT_FALSE(light->getAnchor().isCameraFunction());

        ASSERT_FALSE(light->getIntensity().isUndefined());
        ASSERT_TRUE(light->getIntensity().isConstant());
        ASSERT_EQ(light->getIntensity().asConstant(), 0.3f);
        ASSERT_FALSE(light->getAnchor().isCameraFunction());

        ASSERT_FALSE(light->getColor().isUndefined());
        ASSERT_FALSE(light->getColor().isConstant());
        ASSERT_TRUE(light->getColor().isCameraFunction());

        ASSERT_FALSE(light->getPosition().isUndefined());
        ASSERT_TRUE(light->getPosition().isConstant());
        std::array<float, 3> expected{{ 3, 90, 90 }};
        ASSERT_EQ(light->getPosition().asConstant(), mbgl::style::Position({ expected }));
        ASSERT_FALSE(light->getPosition().isCameraFunction());
    }

    {
        auto light = parseLight(R"({"color":"blue","intensity":0.3,"color-transition":{"duration":1000}})");
        ASSERT_TRUE((bool) light);

        ASSERT_FALSE(light->getColor().isUndefined());
        ASSERT_TRUE(light->getColor().isConstant());
        ASSERT_FALSE(light->getColor().isCameraFunction());
        ASSERT_EQ(light->getColorTransition().duration, mbgl::Duration(mbgl::Milliseconds(1000)));
        ASSERT_FALSE((bool) light->getColorTransition().delay);
    }

    {
        auto light = parseLight(R"({"intensity":false})");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a number", error.message);
    }

    {
        auto light = parseLight(R"({"intensity":{"stops":[[15,"red"],[17,"blue"]]}})");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a number", error.message);
    }

    {
        auto light = parseLight(R"({"color":5})");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a string", error.message);
    }

    {
        auto light = parseLight(R"({"position":[0,5]})");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be an array of 3 numbers", error.message);
    }

    {
        auto light = parseLight(R"({"anchor":"something"})");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a valid enumeration value", error.message);
    }
}
