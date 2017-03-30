#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/position.hpp>

#include <array>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(StyleConversion, Light) {
    Error error;

    auto parseLight = [&](const std::string& src) {
        JSDocument doc;
        doc.Parse<0>(src);
        return convert<Light>(doc, error);
    };

    {
        auto light = parseLight("{}");
        ASSERT_TRUE((bool) light);
    }

    {
        auto light = parseLight("{\"color\":{\"stops\":[[14,\"blue\"],[16,\"red\"]]},\"intensity\":0.3,\"position\":[3,90,90]}");
        ASSERT_TRUE((bool) light);

        ASSERT_TRUE(light->get<LightAnchor>().isUndefined());
        ASSERT_FALSE(light->get<LightAnchor>().isConstant());
        ASSERT_FALSE(light->get<LightAnchor>().isCameraFunction());

        ASSERT_FALSE(light->get<LightIntensity>().isUndefined());
        ASSERT_TRUE(light->get<LightIntensity>().isConstant());
        ASSERT_EQ(light->get<LightIntensity>().asConstant(), 0.3f);
        ASSERT_FALSE(light->get<LightAnchor>().isCameraFunction());

        ASSERT_FALSE(light->get<LightColor>().isUndefined());
        ASSERT_FALSE(light->get<LightColor>().isConstant());
        ASSERT_TRUE(light->get<LightColor>().isCameraFunction());

        ASSERT_FALSE(light->get<LightPosition>().isUndefined());
        ASSERT_TRUE(light->get<LightPosition>().isConstant());
        ASSERT_EQ(light->get<LightPosition>().asConstant(), mbgl::style::Position({{{ 3, 90, 90 }}}));
        ASSERT_FALSE(light->get<LightPosition>().isCameraFunction());
    }

    {
        auto light = parseLight("{\"intensity\":false}");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a number", error.message);
    }

    {
        auto light = parseLight("{\"intensity\":{\"stops\":[[15,\"red\"],[17,\"blue\"]]}}");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a number", error.message);
    }

    {
        auto light = parseLight("{\"color\":5}");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a string", error.message);
    }

    {
        auto light = parseLight("{\"position\":[0,5]}");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be an array of three numbers", error.message);
    }

    {
        auto light = parseLight("{\"anchor\":\"something\"}");

        ASSERT_FALSE((bool) light);
        ASSERT_EQ("value must be a valid enumeration value", error.message);
    }
}
