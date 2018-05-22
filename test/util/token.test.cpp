#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/util/token.hpp>

using namespace mbgl;

TEST(Token, replaceTokens) {
    EXPECT_EQ("literal", mbgl::util::replaceTokens("literal", [](const std::string& token) -> std::string {
        if (token == "name") return "14th St NW";
        return "";
    }));
    EXPECT_EQ("14th St NW", mbgl::util::replaceTokens("{name}", [](const std::string& token) -> std::string {
        if (token == "name") return "14th St NW";
        return "";
    }));
    EXPECT_EQ("", mbgl::util::replaceTokens("{name}", [](const std::string& token) -> std::string {
        if (token == "text") return "14th St NW";
        return "";
    }));
    EXPECT_EQ("1400", mbgl::util::replaceTokens("{num}", [](const std::string& token) -> std::string {
        if (token == "num") return "1400";
        return "";
    }));
    EXPECT_EQ("500 m", mbgl::util::replaceTokens("{num} m", [](const std::string& token) -> std::string {
        if (token == "num") return "500";
        return "";
    }));
    EXPECT_EQ("3 Fine Fields", mbgl::util::replaceTokens("{a} {b} {c}", [](const std::string& token) -> std::string {
        if (token == "a") return "3";
        if (token == "b") return "Fine";
        if (token == "c") return "Fields";
        return "";
    }));
    EXPECT_EQ(" but still", mbgl::util::replaceTokens("{notset} but still", [](const std::string&) -> std::string {
        return "";
    }));
    EXPECT_EQ("dashed", mbgl::util::replaceTokens("{dashed-property}", [](const std::string& token) -> std::string {
        if (token == "dashed-property") return "dashed";
        return "";
    }));
    EXPECT_EQ("colonized", mbgl::util::replaceTokens("{colon:property}", [](const std::string& token) -> std::string {
        if (token == "colon:property") return "colonized";
        return "";
    }));
    EXPECT_EQ("150 m", mbgl::util::replaceTokens("{HØYDE} m", [](const std::string& token) -> std::string {
        if (token == "HØYDE") return "150";
        return "";
    }));
    EXPECT_EQ("{unknown}", mbgl::util::replaceTokens("{unknown}", [](const std::string&) -> optional<std::string> {
        return {};
    }));
}
