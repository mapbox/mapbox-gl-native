#include "gtest/gtest.h"

#include <llmr/map/map.hpp>

#include "../common/headless_view.hpp"

#include <iostream>
#include <fstream>

using namespace llmr;

TEST(Style, Color) {
    // Setup OpenGL
    llmr::HeadlessView view;
    llmr::Map map(view);

    std::ifstream stylefile("./style-fuzzed.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    map.setStyleJSON(stylejson.str());

    // ASSERT_EQ(std::string(""), styles[0]->clip.mask);
    // ASSERT_EQ(3, styles[0]->clip.length);
}
