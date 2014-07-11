#include "gtest/gtest.h"

#include <llmr/map/map.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/std.hpp>

#include <rapidjson/document.h>

#include "../common/headless_view.hpp"

#include "./fixtures/fixture_log.hpp"

#include <dirent.h>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn + "/fixtures/styles";
}();

class HeadlessTest : public ::testing::TestWithParam<std::string> {};

TEST_P(HeadlessTest, render) {
    using namespace llmr;

    const std::string &base = GetParam();

    const std::string style = util::read_file(base_directory + "/" + base + ".style.json");
    const std::string info = util::read_file(base_directory + "/" + base + ".info.json");

    // Parse settings.
    rapidjson::Document doc;
    doc.Parse<0>((const char *const)info.c_str());
    ASSERT_EQ(false, doc.HasParseError());
    ASSERT_EQ(true, doc.IsObject());

    // Setup OpenGL
    HeadlessView view;
    Map map(view);

    for (auto it = doc.MemberBegin(), end = doc.MemberEnd(); it != end; it++) {
        const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

        const std::string name { it->name.GetString(), it->name.GetStringLength() };
        const rapidjson::Value &value = it->value;
        ASSERT_EQ(true, value.IsObject());
        if (value.HasMember("center")) ASSERT_EQ(true, value["center"].IsArray());

        const std::string actual_image = base_directory + "/" + base + "/" + name +  ".actual.png";

        const double zoom = value.HasMember("zoom") ? value["zoom"].GetDouble() : 0;
        const double bearing = value.HasMember("bearing") ? value["bearing"].GetDouble() : 0;
        const double latitude = value.HasMember("center") ? value["center"][rapidjson::SizeType(0)].GetDouble() : 0;
        const double longitude = value.HasMember("center") ? value["center"][rapidjson::SizeType(1)].GetDouble() : 0;
        const unsigned int width = value.HasMember("width") ? value["width"].GetUint() : 512;
        const unsigned int height = value.HasMember("height") ? value["height"].GetUint() : 512;
        std::vector<std::string> classes;
        if (value.HasMember("classes")) {
            const rapidjson::Value &js_classes = value["classes"];
            ASSERT_EQ(true, js_classes.IsArray());
            for (rapidjson::SizeType i = 0; i < js_classes.Size(); i++) {
                const rapidjson::Value &js_class = js_classes[i];
                ASSERT_EQ(true, js_class.IsString());
                classes.push_back({ js_class.GetString(), js_class.GetStringLength() });
            }
        }

        map.setStyleJSON(style);
        map.setAppliedClasses(classes);

        view.resize(width, height);
        map.resize(width, height);
        map.setLonLatZoom(longitude, latitude, zoom);
        map.setAngle(bearing);

        // Run the loop. It will terminate when we don't have any further listeners.
        map.run();

        const std::unique_ptr<uint32_t[]> pixels(new uint32_t[width * height]);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

        const std::string image = util::compress_png(width, height, pixels.get(), true);
        util::write_file(actual_image, image);

        if (value.HasMember("log")) {
            const rapidjson::Value &js_log = value["log"];
            ASSERT_EQ(true, js_log.IsArray());
            for (rapidjson::SizeType i = 0; i < js_log.Size(); i++) {
                const rapidjson::Value &js_entry = js_log[i];
                ASSERT_EQ(true, js_entry.IsArray());
                if (js_entry.Size() == 5) {
                    const uint32_t count = js_entry[rapidjson::SizeType(0)].GetUint();
                    const FixtureLogBackend::LogMessage message {
                        parseEventSeverity(js_entry[rapidjson::SizeType(1)].GetString()),
                        parseEvent(js_entry[rapidjson::SizeType(2)].GetString()),
                        js_entry[rapidjson::SizeType(3)].GetInt64(),
                        js_entry[rapidjson::SizeType(4)].GetString()
                    };
                    ASSERT_EQ(count, log.count(message)) << "Message: " << message << "Full Log: " << std::endl << log.messages;
                } else if (js_entry.Size() == 4) {
                    const uint32_t count = js_entry[rapidjson::SizeType(0)].GetUint();
                    if (js_entry[rapidjson::SizeType(3)].IsString()) {
                        const FixtureLogBackend::LogMessage message {
                            parseEventSeverity(js_entry[rapidjson::SizeType(1)].GetString()),
                            parseEvent(js_entry[rapidjson::SizeType(2)].GetString()),
                            js_entry[rapidjson::SizeType(3)].GetString()
                        };
                        ASSERT_EQ(count, log.count(message)) << "Message: " << message << "Full Log: " << std::endl << log.messages;
                    } else {
                        const FixtureLogBackend::LogMessage message {
                            parseEventSeverity(js_entry[rapidjson::SizeType(1)].GetString()),
                            parseEvent(js_entry[rapidjson::SizeType(2)].GetString()),
                             js_entry[rapidjson::SizeType(3)].GetInt64()
                        };
                        ASSERT_EQ(count, log.count(message)) << "Message: " << message << "Full Log: " << std::endl << log.messages;
                    }
                } else if (js_entry.Size() == 3) {
                    const uint32_t count = js_entry[rapidjson::SizeType(0)].GetUint();
                    const FixtureLogBackend::LogMessage message {
                        parseEventSeverity(js_entry[rapidjson::SizeType(1)].GetString()),
                        parseEvent(js_entry[rapidjson::SizeType(2)].GetString())
                    };
                    ASSERT_EQ(count, log.count(message)) << "Message: " << message << "Full Log: " << std::endl << log.messages;
                } else {
                    FAIL();
                }
            }
        }

        const auto &unchecked = log.unchecked();
        if (unchecked.size()) {
            std::cerr << "Unchecked Log Messages (" << base << "/" << name << "): " << std::endl << unchecked;
        }
    }

}

INSTANTIATE_TEST_CASE_P(Headless, HeadlessTest, ::testing::ValuesIn([] {
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
