#include "gtest/gtest.h"

#include <llmr/map/map.hpp>
#include <llmr/util/image.hpp>
#include <llmr/util/io.hpp>
#include <llmr/util/timer.hpp>

#include "../common/headless_view.hpp"

#include <iostream>
#include <fstream>

TEST(Headless, initialize) {
    const int width = 1024;
    const int height = 768;

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

    map.setLonLatZoom(0, 0, 2);
    map.setAngle(0);
    map.setDebug(false);

    timer.report("map style");

    // Run the loop. It will terminate when we don't have any further listeners.
    map.run();

    timer.report("map loop");

    uint32_t *pixels = new uint32_t[width * height];

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    timer.report("gl readpixels");

    std::string result = llmr::util::compress_png(width, height, pixels, true);

    timer.report("compress png");

    llmr::util::write_file("out.png", result);

    timer.report("save file");

    delete[] pixels;

    timer.report("destruct");
}
