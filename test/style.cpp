#include "gtest/gtest.h"

#include <llmr/style/style.hpp>

#include <iostream>
#include <fstream>

using namespace llmr;

TEST(Style, Color) {
    std::ifstream stylefile("./style-fuzzed.min.js");
    ASSERT_FALSE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}
