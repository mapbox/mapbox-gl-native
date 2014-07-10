#include "gtest/gtest.h"

#include <llmr/style/style.hpp>

#include <iostream>
#include <fstream>

using namespace llmr;

TEST(Style, Style) {
    std::ifstream stylefile("./style.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}

TEST(Style, Colors) {
    std::ifstream stylefile("./fuzz-colors.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}
