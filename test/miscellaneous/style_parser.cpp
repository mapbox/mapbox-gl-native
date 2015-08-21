#include "../fixtures/util.hpp"

#include <mbgl/style/style_parser.hpp>
#include <mbgl/util/io.hpp>

#include <mbgl/map/mode.hpp>
#include <mbgl/map/map_data.hpp>

#include <rapidjson/document.h>

#include "../fixtures/fixture_log_observer.hpp"

#include <iostream>
#include <fstream>

#include <dirent.h>

using namespace mbgl;

typedef std::pair<uint32_t, std::string> Message;
typedef std::vector<Message> Messages;

class StyleParserTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StyleParserTest, ParseStyle) {
    const std::string &base = "test/fixtures/style_parser/" + GetParam();

    rapidjson::Document infoDoc;
    infoDoc.Parse<0>(util::read_file(base + ".info.json").c_str());
    ASSERT_FALSE(infoDoc.HasParseError());
    ASSERT_TRUE(infoDoc.IsObject());

    rapidjson::Document styleDoc;
    styleDoc.Parse<0>(util::read_file(base + ".style.json").c_str());
    ASSERT_FALSE(styleDoc.HasParseError());
    ASSERT_TRUE(styleDoc.IsObject());

    FixtureLogObserver* observer = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(observer));

    MapMode mapMode = MapMode::Continuous;
    const float pixelRatio = 1.0f;

    MapData data(mapMode, pixelRatio);
    StyleParser parser(data);
    parser.parse(styleDoc);

    for (auto it = infoDoc.MemberBegin(), end = infoDoc.MemberEnd(); it != end; it++) {
        const std::string name { it->name.GetString(), it->name.GetStringLength() };
        const rapidjson::Value &value = it->value;
        ASSERT_EQ(true, value.IsObject());

        if (value.HasMember("log")) {
            const rapidjson::Value &js_log = value["log"];
            ASSERT_EQ(true, js_log.IsArray());
            for (rapidjson::SizeType i = 0; i < js_log.Size(); i++) {
                const rapidjson::Value &js_entry = js_log[i];
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
