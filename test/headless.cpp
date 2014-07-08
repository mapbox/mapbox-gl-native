#include "gtest/gtest.h"

#include <llmr/map/map.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/timer.hpp>

#include "../common/headless_view.hpp"

#include <iostream>
#include <fstream>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn;
}();

TEST(Headless, initialize) {
    const unsigned int width = 1024;
    const unsigned int height = 768;

    llmr::util::timer timer;

    // Setup OpenGL
    llmr::HeadlessView view;
    llmr::Map map(view);

    view.resize(width, height);
    map.resize(width, height);

    timer.report("map setup");

    std::ifstream stylefile("./style.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    map.setStyleJSON(stylejson.str());

    timer.report("map style");

    map.setLonLatZoom(0, 0, 2);
    map.setAngle(0);
    map.setDebug(false);

    // Run the loop. It will terminate when we don't have any further listeners.
    map.run();

    timer.report("map loop");

    uint32_t *pixels = new uint32_t[width * height];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    std::string result = llmr::util::compress_png(width, height, pixels, true);
    llmr::util::write_file("out.png", result);

    std::ifstream imagefile(base_directory + "/fixtures/result/out.png");
    ASSERT_TRUE(imagefile.good());
    std::stringstream image;
    image << imagefile.rdbuf();
    llmr::util::Image reference(image.str(), true);

    ASSERT_EQ(reference.getWidth(), width);
    ASSERT_EQ(reference.getHeight(), height);
    ASSERT_EQ(0, std::memcmp(pixels, reference.getData(), width * height * 4));

    delete[] pixels;
}
