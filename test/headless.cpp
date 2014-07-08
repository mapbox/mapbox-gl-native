#include "gtest/gtest.h"

#include <llmr/map/map.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/std.hpp>

#include <rapidjson/document.h>

#include "../common/headless_view.hpp"

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn;
}();

class HeadlessTest : public ::testing::TestWithParam<const char *> {};

TEST_P(HeadlessTest, render) {
    const char *base = GetParam();

    const std::string style = llmr::util::read_file(base_directory + "/fixtures/styles/" + base + ".style.json");
    const std::string info = llmr::util::read_file(base_directory + "/fixtures/styles/" + base + ".info.json");
    const std::string expected_image = base_directory + "/fixtures/styles/" + base + ".expected.png";
    const std::string actual_image = base_directory + "/fixtures/styles/" + base + ".actual.png";

    // Parse settings.
    rapidjson::Document doc;
    doc.Parse<0>((const char *const)info.c_str());
    ASSERT_EQ(false, doc.HasParseError());

    const double zoom = doc.HasMember("zoom") ? doc["zoom"].GetDouble() : 0;
    const double angle = doc.HasMember("angle") ? doc["angle"].GetDouble() : 0;
    const double longitude = doc.HasMember("longitude") ? doc["longitude"].GetDouble() : 0;
    const double latitude = doc.HasMember("latitude") ? doc["latitude"].GetDouble() : 0;
    const unsigned int width = doc.HasMember("width") ? doc["width"].GetUint() : 0;
    const unsigned int height = doc.HasMember("height") ? doc["height"].GetUint() : 0;

    // Setup OpenGL
    llmr::HeadlessView view;
    llmr::Map map(view);

    view.resize(width, height);
    map.resize(width, height);

    map.setStyleJSON(style);

    map.setLonLatZoom(longitude, latitude, zoom);
    map.setAngle(angle);
    map.setDebug(false);

    // Run the loop. It will terminate when we don't have any further listeners.
    map.run();

    const std::unique_ptr<uint32_t[]> pixels(new uint32_t[width * height]);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

    const std::string image = llmr::util::compress_png(width, height, pixels.get(), true);
    llmr::util::write_file(actual_image, image);

    const std::string expected_image_data(llmr::util::read_file(expected_image));
    const llmr::util::Image expected(expected_image_data, true);
    ASSERT_EQ(width, expected.getWidth());
    ASSERT_EQ(height, expected.getHeight());
    ASSERT_EQ(0, std::memcmp(pixels.get(), expected.getData(), width * height * 4));
}

INSTANTIATE_TEST_CASE_P(Headless, HeadlessTest,
                        ::testing::Values("0"));
