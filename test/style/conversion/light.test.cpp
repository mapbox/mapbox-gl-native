#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/light.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/rapidjson.hpp>

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

        const mbgl::JSValue colorValue("blue");
        light->setProperty("color", &colorValue);

        ASSERT_FALSE(light->getColor().isUndefined());
        ASSERT_TRUE(light->getColor().isConstant());
        ASSERT_EQ(light->getColor().asConstant(), mbgl::Color::blue());

        const mbgl::JSValue intensityValue(0.5);
        light->setProperty("intensity", &intensityValue);
        ASSERT_FALSE(light->getIntensity().isUndefined());
        ASSERT_TRUE(light->getIntensity().isConstant());
        ASSERT_EQ(light->getIntensity().asConstant(), 0.5);

        mbgl::JSValue::AllocatorType allocator;
        const mbgl::JSValue positionValue(std::move(mbgl::JSValue(rapidjson::kArrayType).PushBack(1.f, allocator).PushBack(2.f, allocator).PushBack(3.f, allocator).Move()));
        light->setProperty("position", &positionValue);
        ASSERT_FALSE(light->getPosition().isUndefined());
        ASSERT_TRUE(light->getPosition().isConstant());
        std::array<float, 3> expected{{ 1.f, 2.f, 3.f }};
        ASSERT_EQ(light->getPosition().asConstant(), mbgl::style::Position({ expected }));
    }

    {
        auto light = parseLight(R"({"color":{"stops":[[14,"blue"],[16,"red"]]},"intensity":0.3,"position":[3,90,90]})");
        ASSERT_TRUE((bool) light);

        ASSERT_TRUE(light->getAnchor().isUndefined());
        ASSERT_FALSE(light->getAnchor().isConstant());
        ASSERT_FALSE(light->getAnchor().isExpression());

        ASSERT_FALSE(light->getIntensity().isUndefined());
        ASSERT_TRUE(light->getIntensity().isConstant());
        ASSERT_EQ(light->getIntensity().asConstant(), 0.3f);
        ASSERT_FALSE(light->getIntensity().isExpression());

        ASSERT_FALSE(light->getColor().isUndefined());
        ASSERT_FALSE(light->getColor().isConstant());
        ASSERT_TRUE(light->getColor().isExpression());
        ASSERT_FALSE(light->getColor().asExpression().isZoomConstant());

        ASSERT_FALSE(light->getPosition().isUndefined());
        ASSERT_TRUE(light->getPosition().isConstant());
        std::array<float, 3> expected{{ 3, 90, 90 }};
        ASSERT_EQ(light->getPosition().asConstant(), mbgl::style::Position({ expected }));
        ASSERT_FALSE(light->getPosition().isExpression());
    }

    {
        auto light = parseLight(R"({"color":"blue","intensity":0.3,"color-transition":{"duration":1000}})");
        ASSERT_TRUE((bool) light);

        ASSERT_FALSE(light->getColor().isUndefined());
        ASSERT_TRUE(light->getColor().isConstant());
        ASSERT_FALSE(light->getColor().isExpression());
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
