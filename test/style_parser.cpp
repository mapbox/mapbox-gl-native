#include "gtest/gtest.h"

#include <mbgl/style/style.hpp>
#include <mbgl/util/io.hpp>

#include <rapidjson/document.h>

#include "./fixtures/fixture_log.hpp"

#include <iostream>
#include <fstream>

#include <dirent.h>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn + "/fixtures/style_parser";
}();

using namespace mbgl;

typedef std::pair<uint32_t, std::string> Message;
typedef std::vector<Message> Messages;

class StyleParserTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StyleParserTest, ParseStyle) {
    const std::string &base = base_directory + "/" + GetParam();

    const std::string style_path = base + ".style.json";
    const std::string info = util::read_file(base + ".info.json");

    // Parse settings.
    rapidjson::Document doc;
    doc.Parse<0>((const char *const)info.c_str());
    ASSERT_EQ(false, doc.HasParseError());
    ASSERT_EQ(true, doc.IsObject());

    std::ifstream stylefile(style_path);
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    for (auto it = doc.MemberBegin(), end = doc.MemberEnd(); it != end; it++) {
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
                const FixtureLogBackend::LogMessage message {
                    EventSeverityClass(js_entry[rapidjson::SizeType(1)].GetString()),
                    EventClass(js_entry[rapidjson::SizeType(2)].GetString()),
                    js_entry[rapidjson::SizeType(3)].GetString()
                };

                EXPECT_EQ(count, log.count(message)) << "Message: " << message << std::endl;
            }
        }

        const auto &unchecked = log.unchecked();
        if (unchecked.size()) {
            std::cerr << "Unchecked Log Messages (" << base << "/" << name << "): " << std::endl << unchecked;
        }

        ASSERT_EQ(0ul, unchecked.size());
    }
}

INSTANTIATE_TEST_CASE_P(StyleParser, StyleParserTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;
    const std::string ending = ".info.json";

    DIR *dir = opendir(base_directory.c_str());
    if (dir == nullptr) {
        return names;
    }

    for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
        const std::string name = dp->d_name;
        if (name.length() >= ending.length() && name.compare(name.length() - ending.length(), ending.length(), ending) == 0) {
            names.push_back(name.substr(0, name.length() - ending.length()));
        }
    }

    closedir(dir);

    return names;
}()));
