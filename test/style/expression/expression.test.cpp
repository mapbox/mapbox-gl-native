#include <mbgl/test/util.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/expression/is_expression.hpp>

#include <rapidjson/document.h>

#include <iostream>
#include <fstream>
#include <dirent.h>


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

class ExpressionEqualityTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ExpressionEqualityTest, ExpressionEquality) {
    const std::string base = std::string("test/fixtures/expression_equality/") + GetParam();

    std::string error;
    auto parse = [&](std::string filename, std::string& error_) -> std::unique_ptr<expression::Expression> {
        rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
        document.Parse<0>(util::read_file(filename).c_str());
        assert(!document.HasParseError());
        const JSValue* expression = &document;
        expression::ParsingContext ctx;
        expression::ParseResult parsed = ctx.parse(conversion::Convertible(expression));
        if (!parsed) {
            error_ = ctx.getErrors().size() > 0 ? ctx.getErrors()[0].message : "failed to parse";
        };
        return std::move(*parsed);
    };

    std::unique_ptr<expression::Expression> expression_a1 = parse(base + ".a.json", error);
    ASSERT_TRUE(expression_a1) << GetParam() << ": " << error;

    std::unique_ptr<expression::Expression> expression_a2 = parse(base + ".a.json", error);
    ASSERT_TRUE(expression_a2) << GetParam() << ": " << error;

    std::unique_ptr<expression::Expression> expression_b = parse(base + ".b.json", error);
    ASSERT_TRUE(expression_b) << GetParam() << ": " << error;


    EXPECT_TRUE(*expression_a1 == *expression_a2);
    EXPECT_TRUE(*expression_a1 != *expression_b);
}

INSTANTIATE_TEST_CASE_P(Expression, ExpressionEqualityTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;
    const std::string ending = ".a.json";

    const std::string style_directory = "test/fixtures/expression_equality";
    DIR *dir = opendir(style_directory.c_str());
    if (dir != nullptr) {
        for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
            const std::string name = dp->d_name;
            if (name.length() >= ending.length() && name.compare(name.length() - ending.length(), ending.length(), ending) == 0) {
                names.push_back(name.substr(0, name.length() - ending.length()));
            }
        }
        closedir(dir);
    }

    EXPECT_GT(names.size(), 0u);
    return names;
}()));
