#include "gtest/gtest.h"

#include <mbgl/style/style.hpp>
#include "./fixtures/fixture_log.hpp"

#include <iostream>
#include <fstream>

using namespace mbgl;

typedef std::vector<std::pair<uint32_t, std::string>> Messages;

void checkMessages(const FixtureLogBackend &log, Messages messages) {
    for (auto &it : messages) {
        const FixtureLogBackend::LogMessage message {
            EventSeverityClass("WARNING"),
            EventClass("ParseStyle"),
            it.second
        };

        EXPECT_EQ(it.first, log.count(message)) << "Message: "
            << message << std::endl;
    }

    const auto &unchecked = log.unchecked();
    if (unchecked.size()) {
        std::cerr << "Unchecked Log Messages: " << std::endl 
            << unchecked;
    }

    ASSERT_EQ(0, unchecked.size());
}

TEST(Style, Style) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./style.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    checkMessages(log, {});
}

TEST(Style, Colors) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-colors.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    checkMessages(log, {
        {84, "value of 'line-width' must be a number, or a number function"},
        {6, "value of 'line-opacity' must be a number, or a number function"},
        {12, "value of 'text-size' must be a number, or a number function"}
    });
}

TEST(Style, Functions) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-functions.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    checkMessages(log, {
        {28, "stop must have zoom level and value specification"},
        {12, "function must specify a function type"}
    });
}

TEST(Style, Layers) {
    const FixtureLogBackend &log = Log::Set<FixtureLogBackend>();

    std::ifstream stylefile("./fuzz-layers.min.js");
    ASSERT_TRUE(stylefile.good());
    std::stringstream stylejson;
    stylejson << stylefile.rdbuf();

    Style style;
    style.loadJSON((const uint8_t *)stylejson.str().c_str());

    checkMessages(log, {
        {20, "stop must have zoom level and value specification"},
        {6, "function must specify a function type"},
        {1, "value of 'line-width' must be a number, or a number function"},
        {1, "array value has unexpected number of elements"},
        {1, "value of 'line-opacity' must be a number, or a number function"}
    });
}
