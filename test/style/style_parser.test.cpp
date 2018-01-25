#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/style/parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/tileset.hpp>

#include <rapidjson/document.h>

#include <iostream>
#include <fstream>

#include <dirent.h>

using namespace mbgl;

using Message = std::pair<uint32_t, std::string>;
using Messages = std::vector<Message>;

class StyleParserTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StyleParserTest, ParseStyle) {
    const std::string base = std::string("test/fixtures/style_parser/") + GetParam();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> infoDoc;
    infoDoc.Parse<0>(util::read_file(base + ".info.json").c_str());
    ASSERT_FALSE(infoDoc.HasParseError());
    ASSERT_TRUE(infoDoc.IsObject());

    auto observer = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(observer));

    style::Parser parser;
    auto error = parser.parse(util::read_file(base + ".style.json"));

    if (error) {
        Log::Error(Event::ParseStyle, "Failed to parse style: %s", util::toString(error).c_str());
    }

    ASSERT_TRUE(infoDoc.IsObject());
    for (const auto& property : infoDoc.GetObject()) {
        const std::string name { property.name.GetString(), property.name.GetStringLength() };
        const JSValue &value = property.value;
        ASSERT_EQ(true, value.IsObject());

        if (value.HasMember("log")) {
            const JSValue &js_log = value["log"];
            ASSERT_EQ(true, js_log.IsArray());
            for (auto& js_entry : js_log.GetArray()) {
                ASSERT_EQ(true, js_entry.IsArray());
                ASSERT_GE(4u, js_entry.Size());

                const uint32_t count = js_entry[rapidjson::SizeType(0)].GetUint();
                const FixtureLogObserver::LogMessage message {
                    *Enum<EventSeverity>::toEnum(js_entry[rapidjson::SizeType(1)].GetString()),
                    *Enum<Event>::toEnum(js_entry[rapidjson::SizeType(2)].GetString()),
                    int64_t(-1),
                    js_entry[rapidjson::SizeType(3)].GetString()
                };

                EXPECT_EQ(count, observer->count(message)) << "Message: " << message << std::endl;
            }
        }

        const auto &unchecked = observer->unchecked();
        if (unchecked.size()) {
            std::cerr << "Unchecked Log Messages (" << base << "/" << name << "): " << std::endl << unchecked;
        }

        ASSERT_EQ(0u, unchecked.size());
    }
}

INSTANTIATE_TEST_CASE_P(StyleParser, StyleParserTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;
    const std::string ending = ".info.json";

    const std::string style_directory = "test/fixtures/style_parser";
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

TEST(StyleParser, FontStacks) {
    style::Parser parser;
    parser.parse(util::read_file("test/fixtures/style_parser/font_stacks.json"));
    auto result = parser.fontStacks();
    ASSERT_EQ(3u, result.size());
    ASSERT_EQ(FontStack({"a"}), result[0]);
    ASSERT_EQ(FontStack({"a", "b"}), result[1]);
    ASSERT_EQ(FontStack({"a", "b", "c"}), result[2]);
}

TEST(StyleParser, FontStacksNoTextField) {
    style::Parser parser;
    parser.parse(R"({
        "version": 8,
        "layers": [{
            "id": "symbol",
            "type": "symbol",
            "source": "vector",
            "layout": {
                "text-font": ["a"]
            }
        }]
    })");
    auto result = parser.fontStacks();
    ASSERT_EQ(0u, result.size());
}

TEST(StyleParser, FontStacksCaseExpression) {
    style::Parser parser;
    parser.parse(R"({
        "version": 8,
        "layers": [{
            "id": "symbol",
            "type": "symbol",
            "source": "vector",
            "layout": {
                "text-field": "a",
                "text-font": ["case", ["==", "a", ["string", ["get", "text-font"]]], ["literal", ["Arial"]], ["literal", ["Helvetica"]]]
            }
        }]
    })");
    auto result = parser.fontStacks();
    ASSERT_EQ(2u, result.size());
    ASSERT_EQ(FontStack({"Arial"}), result[0]);
    ASSERT_EQ(FontStack({"Helvetica"}), result[1]);
}

TEST(StyleParser, FontStacksMatchExpression) {
    style::Parser parser;
    parser.parse(R"({
        "version": 8,
        "layers": [{
            "id": "symbol",
            "type": "symbol",
            "source": "vector",
            "layout": {
                "text-field": "a",
                "text-font": ["match", ["get", "text-font"], "a", ["literal", ["Arial"]], ["literal", ["Helvetica"]]]
            }
        }]
    })");
    auto result = parser.fontStacks();
    ASSERT_EQ(2u, result.size());
    ASSERT_EQ(FontStack({"Arial"}), result[0]);
    ASSERT_EQ(FontStack({"Helvetica"}), result[1]);
}

TEST(StyleParser, FontStacksStepExpression) {
    style::Parser parser;
    parser.parse(R"({
        "version": 8,
        "layers": [{
            "id": "symbol",
            "type": "symbol",
            "source": "vector",
            "layout": {
                "text-field": "a",
                "text-font": ["array", "string", ["step", ["get", "text-font"], ["literal", ["Arial"]], 0, ["literal", ["Helvetica"]]]]
            }
        }]
    })");
    auto result = parser.fontStacks();
    ASSERT_EQ(2u, result.size());
    ASSERT_EQ(FontStack({"Arial"}), result[0]);
    ASSERT_EQ(FontStack({"Helvetica"}), result[1]);
}

TEST(StyleParser, FontStacksGetExpression) {
    // Invalid style, but not currently validated.
    style::Parser parser;
    parser.parse(R"({
        "version": 8,
        "layers": [{
            "id": "symbol",
            "type": "symbol",
            "source": "vector",
            "layout": {
                "text-field": "a",
                "text-font": ["array", "string", ["get", "text-font"]]
            }
        }]
    })");
    auto result = parser.fontStacks();
    ASSERT_EQ(0u, result.size());
}
