#include "gtest/gtest.h"

#include <mbgl/map/map.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/std.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "../common/headless_view.hpp"

#include "./fixtures/fixture_log.hpp"

#include <dirent.h>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn + "/../node_modules/mapbox-gl-test-suite/";
}();

class HeadlessTest : public ::testing::TestWithParam<std::string> {};

void ResolveLocalURL(rapidjson::Value& value, rapidjson::Document& doc) {
    std::string str { value.GetString(), value.GetStringLength() };
    str.replace(0, 8, base_directory); // local://
    value.SetString(str.c_str(), str.length(), doc.GetAllocator());
}

TEST_P(HeadlessTest, render) {
    using namespace mbgl;

    const std::string &base = GetParam();

    std::string style = util::read_file(base_directory + "tests/" + base + "/style.json");
    std::string info = util::read_file(base_directory + "tests/" + base + "/info.json");

    // Parse style.
    rapidjson::Document styleDoc;
    styleDoc.Parse<0>((const char *const)style.c_str());
    ASSERT_EQ(false, styleDoc.HasParseError());
    ASSERT_EQ(true, styleDoc.IsObject());

    if (styleDoc.HasMember("sprite")) {
        ResolveLocalURL(styleDoc["sprite"], styleDoc);
    }

    ResolveLocalURL(styleDoc["sources"]["mapbox"]["tiles"][rapidjson::SizeType(0)], styleDoc);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    styleDoc.Accept(writer);
    style = buffer.GetString();

    // Parse settings.
    rapidjson::Document infoDoc;
    infoDoc.Parse<0>((const char *const)info.c_str());
    ASSERT_EQ(false, infoDoc.HasParseError());
    ASSERT_EQ(true, infoDoc.IsObject());

    Log::Set<FixtureLogBackend>();

    // Setup OpenGL
    HeadlessView view;
    Map map(view);

    for (auto it = infoDoc.MemberBegin(), end = infoDoc.MemberEnd(); it != end; it++) {
        const std::string name { it->name.GetString(), it->name.GetStringLength() };
        const rapidjson::Value &value = it->value;
        ASSERT_EQ(true, value.IsObject());
        if (value.HasMember("center")) ASSERT_EQ(true, value["center"].IsArray());

        const std::string actual_image = base_directory + "tests/" + base + "/" + name +  "/actual.png";

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
        map.setBearing(bearing);

        // Run the loop. It will terminate when we don't have any further listeners.
        map.run();

        const std::unique_ptr<uint32_t[]> pixels(new uint32_t[width * height]);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

        const std::string image = util::compress_png(width, height, pixels.get(), true);
        util::write_file(actual_image, image);
    }

}

INSTANTIATE_TEST_CASE_P(Headless, HeadlessTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;

    DIR *dir = opendir((base_directory + "tests").c_str());
    if (dir == nullptr) {
        return names;
    }

    for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
        const std::string name = dp->d_name;
        if (name != "." && name != ".." && name != "index.html") {
            names.push_back(name);
        }
    }

    closedir(dir);

    return names;
}()));
