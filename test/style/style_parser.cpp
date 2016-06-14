#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/style/parser.hpp>
#include <mbgl/util/io.hpp>

#include <rapidjson/document.h>

#include <iostream>
#include <fstream>

#include <dirent.h>

using namespace mbgl;

typedef std::pair<uint32_t, std::string> Message;
typedef std::vector<Message> Messages;

class StyleParserTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StyleParserTest, ParseStyle) {
    const std::string base = std::string("test/fixtures/style_parser/") + GetParam();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> infoDoc;
    infoDoc.Parse<0>(util::read_file(base + ".info.json").c_str());
    ASSERT_FALSE(infoDoc.HasParseError());
    ASSERT_TRUE(infoDoc.IsObject());

    FixtureLogObserver* observer = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(observer));

    style::Parser parser;
    parser.parse(util::read_file(base + ".style.json"));

    for (auto it = infoDoc.MemberBegin(), end = infoDoc.MemberEnd(); it != end; it++) {
        const std::string name { it->name.GetString(), it->name.GetStringLength() };
        const JSValue &value = it->value;
        ASSERT_EQ(true, value.IsObject());

        if (value.HasMember("log")) {
            const JSValue &js_log = value["log"];
            ASSERT_EQ(true, js_log.IsArray());
            for (rapidjson::SizeType i = 0; i < js_log.Size(); i++) {
                const JSValue &js_entry = js_log[i];
                ASSERT_EQ(true, js_entry.IsArray());

                const uint32_t count = js_entry[rapidjson::SizeType(0)].GetUint();
                const FixtureLogObserver::LogMessage message {
                    EventSeverityClass(js_entry[rapidjson::SizeType(1)].GetString()),
                    EventClass(js_entry[rapidjson::SizeType(2)].GetString()),
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

        ASSERT_EQ(0ul, unchecked.size());
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

    EXPECT_GT(names.size(), 0ul);
    return names;
}()));

TEST(StyleParser, ParseTileJSONRaster) {
    auto result = style::parseTileJSON(
        util::read_file("test/fixtures/style_parser/tilejson.raster.json"),
        "mapbox://mapbox.satellite",
        SourceType::Raster,
        256);

    EXPECT_EQ(0, result->minZoom);
    EXPECT_EQ(15, result->maxZoom);
    EXPECT_EQ("attribution", result->attribution);
#if !defined(__ANDROID__) && !defined(__APPLE__)
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.webp", result->tiles[0]);
#else
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", result->tiles[0]);
#endif
}

TEST(StyleParser, ParseTileJSONVector) {
    auto result = style::parseTileJSON(
        util::read_file("test/fixtures/style_parser/tilejson.vector.json"),
        "mapbox://mapbox.streets",
        SourceType::Vector,
        256);

    EXPECT_EQ(0, result->minZoom);
    EXPECT_EQ(15, result->maxZoom);
    EXPECT_EQ("attribution", result->attribution);
    EXPECT_EQ("mapbox://tiles/mapbox.streets/{z}/{x}/{y}.vector.pbf", result->tiles[0]);
}

TEST(StyleParser, FontStacks) {
    style::Parser parser;
    parser.parse(util::read_file("test/fixtures/style_parser/font_stacks.json"));
    auto result = parser.fontStacks();
    ASSERT_EQ(3, result.size());
    ASSERT_EQ(FontStack({"a"}), result[0]);
    ASSERT_EQ(FontStack({"a", "b"}), result[1]);
    ASSERT_EQ(FontStack({"a", "b", "c"}), result[2]);
}
