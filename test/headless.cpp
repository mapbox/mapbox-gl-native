#include "gtest/gtest.h"

#include <llmr/map/map.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/std.hpp>

#include <rapidjson/document.h>

#include "../common/headless_view.hpp"

#include <dirent.h>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn + "/fixtures/styles";
}();

class HeadlessTest : public ::testing::TestWithParam<std::string> {};

TEST_P(HeadlessTest, render) {
    const std::string &base = GetParam();

    const std::string style = llmr::util::read_file(base_directory + "/" + base + ".style.json");
    const std::string info = llmr::util::read_file(base_directory + "/" + base + ".info.json");

    // Parse settings.
    rapidjson::Document doc;
    doc.Parse<0>((const char *const)info.c_str());
    ASSERT_EQ(false, doc.HasParseError());
    ASSERT_EQ(true, doc.IsObject());

    // Setup OpenGL
    llmr::HeadlessView view;
    llmr::Map map(view);

    for (auto it = doc.MemberBegin(), end = doc.MemberEnd(); it != end; it++) {
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

        map.setStyleJSON(style);

        view.resize(width, height);
        map.resize(width, height);
        map.setLonLatZoom(longitude, latitude, zoom);
        map.setAngle(bearing);

        // Run the loop. It will terminate when we don't have any further listeners.
        map.run();

        const std::unique_ptr<uint32_t[]> pixels(new uint32_t[width * height]);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

        const std::string image = llmr::util::compress_png(width, height, pixels.get(), true);
        llmr::util::write_file(actual_image, image);
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
