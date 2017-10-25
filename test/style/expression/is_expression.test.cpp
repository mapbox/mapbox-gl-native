#include <mbgl/test/util.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/expression/is_expression.hpp>

#include <rapidjson/document.h>

using namespace mbgl;
using namespace mbgl::style;

TEST(Expression, IsExpression) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> spec;
    spec.Parse<0>(util::read_file("mapbox-gl-js/src/style-spec/reference/v8.json").c_str());
    ASSERT_FALSE(spec.HasParseError());
    ASSERT_TRUE(spec.IsObject() &&
                spec.HasMember("expression_name") &&
                spec["expression_name"].IsObject() &&
                spec["expression_name"].HasMember("values") &&
                spec["expression_name"]["values"].IsObject());
    
    const auto& allExpressions = spec["expression_name"]["values"];
    
    for(auto& entry : allExpressions.GetObject()) {
        const std::string name { entry.name.GetString(), entry.name.GetStringLength() };
        JSDocument document;
        document.Parse<0>(R"([")" + name + R"("])");

        const JSValue* expression = &document;
        EXPECT_TRUE(expression::isExpression(conversion::Convertible(expression))) << name;
    }
}
