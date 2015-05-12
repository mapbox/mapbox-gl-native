#include <iostream>
#include <unordered_map>
#include "../fixtures/util.hpp"

#include <rapidjson/document.h>
#include <csscolorparser/csscolorparser.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/style/color_operations.hpp>

using namespace mbgl;

void ParsedValid(const rapidjson::Value& op, rapidjson::SizeType size)
{
    ASSERT_TRUE(op.IsArray());
    ASSERT_FALSE(op.IsNull());
    EXPECT_EQ(size, op.Size());
}

void TestColor(CSSColorParser::Color opResult, uint64_t r, uint64_t g, uint64_t b, uint64_t a)
{
    EXPECT_EQ(r, opResult.r);
    EXPECT_EQ(g, opResult.g);
    EXPECT_EQ(b, opResult.b);
    EXPECT_EQ(a, opResult.a);
}

TEST(StyleColorOperation, Evaluate) {

    // parse setup
    const std::string json = util::read_file("test/fixtures/color_operations/doc.json");
    rapidjson::Document document;
    document.Parse<0>((const char *const)json.c_str());

    ASSERT_FALSE(document.HasParseError());
    ASSERT_TRUE(document.IsObject());

    // set up colors and constants map
    const rapidjson::Value& light = document["light"];
    const rapidjson::Value& complex = document["complex"];
    const rapidjson::Value& spin_constant = document["spin_constant"];
    const rapidjson::Value& mix_constant = document["mix_constant"];
    const rapidjson::Value& empty = document["empty"];
    const rapidjson::Value& lacks_op = document["lacks_op"];
    std::unordered_map<std::string, const rapidjson::Value *> constants;
    constants.emplace(std::string("@white"), &document["@white"]);
    constants.emplace(std::string("@white"), &document["@black"]);
    constants.emplace(std::string("@red"), &document["@red"]);
    constants.emplace(std::string("@purple"), &document["@purple"]);

    // validate rapidjson stub parsing
    ParsedValid(light, rapidjson::SizeType(3));
    ParsedValid(complex, rapidjson::SizeType(3));
    ParsedValid(complex[2u], rapidjson::SizeType(4));
    ParsedValid(complex[2u][3u], rapidjson::SizeType(3));
    ParsedValid(spin_constant, rapidjson::SizeType(3));
    ParsedValid(mix_constant, rapidjson::SizeType(4));
    ParsedValid(empty, rapidjson::SizeType(0));
    ParsedValid(lacks_op, rapidjson::SizeType(3));

    // test color operations
    CSSColorParser::Color parsedLight = mbgl::parseColorOp(light, constants);
    TestColor(parsedLight, 128, 128, 128, 1);

    CSSColorParser::Color parsedComplex = mbgl::parseColorOp(complex, constants);
    TestColor(parsedComplex, 71, 2, 9, 1);

    CSSColorParser::Color parsedSpinConst = mbgl::parseColorOp(spin_constant, constants);
    TestColor(parsedSpinConst, 255, 43, 0, 0);

    CSSColorParser::Color parsedMixConst = mbgl::parseColorOp(mix_constant, constants);
    TestColor(parsedMixConst, 119, 20, 111, 1);

    CSSColorParser::Color parsedEmpty = mbgl::parseColorOp(empty, constants);
    TestColor(parsedEmpty, 0, 0, 0, 1);

    CSSColorParser::Color parsedLacksOp = mbgl::parseColorOp(lacks_op, constants);
    TestColor(parsedLacksOp, 0, 0, 0, 1);
}
