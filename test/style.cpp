#include "gtest/gtest.h"

#include <llmr/style/style.hpp>
#include "./fixtures/fixture_log.hpp"

#include <iostream>
#include <fstream>

using namespace llmr;

TEST(Style, Style) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./style.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}

TEST(Style, Colors) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-colors.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}

TEST(Style, Functions) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-functions.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    const FixtureLogBackend::LogMessage number {
        EventSeverityClass("WARNING"),
        EventClass("ParseStyle"),
        "value of 'line-width' must be a number, or a number function"
    };

    ASSERT_EQ(0, log.count(number));

    const auto &unchecked = log.unchecked();
    if (unchecked.size()) {
        std::cerr << "Unchecked Log Messages: " << std::endl 
            << unchecked;
    }

    ASSERT_EQ(0, unchecked.size());
}

TEST(Style, Layers) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-layers.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());
}

