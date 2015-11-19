#include <iostream>
#include "../fixtures/util.hpp"

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
    EXPECT_EQ("150 m", mbgl::util::replaceTokens("{HØYDE} m", [](const std::string& token) -> std::string {
        if (token == "HØYDE") return "150";
        return "";
    }));
    EXPECT_EQ("reserved {for=future} use", mbgl::util::replaceTokens("reserved {for=future} use", [](const std::string& token) -> std::string {
        if (token == "for=future") return "unknown";
        return "";
    }));
    EXPECT_EQ("null", mbgl::util::replaceTokens("{}", [](const std::string& token) -> std::string {
        if (token == "") return "null";
        return "";
    }));
    EXPECT_EQ("", mbgl::util::replaceTokens("{}", [](const std::string&) -> std::string {
        return "";
    }));
    EXPECT_EQ("?", mbgl::util::replaceTokens("{name:?}", [](const std::string&) -> std::string {
        return "";
    }));
    EXPECT_EQ("Louisiana", mbgl::util::replaceTokens("{name_en:{name}}", [](const std::string& token) -> std::string {
        if (token == "name") return "Louisiana";
        return "";
    }));
    EXPECT_EQ("Louisiana", mbgl::util::replaceTokens("{name_en:{name}}", [](const std::string& token) -> std::string {
        if (token == "name") return "Louisiana";
        if (token == "name_fr") return "Louisiane";
        return "";
    }));
    EXPECT_EQ("Louisiane", mbgl::util::replaceTokens("{name_fr:{name}}", [](const std::string& token) -> std::string {
        if (token == "name") return "Louisiana";
        if (token == "name_fr") return "Louisiane";
        return "";
    }));
    EXPECT_EQ("Luisiana", mbgl::util::replaceTokens("{name_en:{name_es:{name:?}}}", [](const std::string& token) -> std::string {
        if (token == "name") return "Lwizyàn";
        if (token == "name_es") return "Luisiana";
        return "";
    }));
    EXPECT_EQ("(Luisiana)", mbgl::util::replaceTokens("({name_en:{name_es:{name:?}}})", [](const std::string& token) -> std::string {
        if (token == "name") return "Lwizyàn";
        if (token == "name_es") return "Luisiana";
        return "";
    }));
    EXPECT_EQ("Lwizyàn", mbgl::util::replaceTokens("{name_en:{name_es:{name:?}}}", [](const std::string& token) -> std::string {
        if (token == "name") return "Lwizyàn";
        return "";
    }));
    EXPECT_EQ("", mbgl::util::replaceTokens("{name_en:}", [](const std::string& token) -> std::string {
        if (token == "name") return "Louisiana";
        return "";
    }));
    EXPECT_EQ("Louisiana", mbgl::util::replaceTokens("{name:}", [](const std::string& token) -> std::string {
        if (token == "name") return "Louisiana";
        return "";
    }));
    EXPECT_EQ("name", mbgl::util::replaceTokens("{:name}", [](const std::string&) -> std::string {
        return "";
    }));
    EXPECT_EQ("", mbgl::util::replaceTokens("{:}", [](const std::string&) -> std::string {
        return "";
    }));
}
