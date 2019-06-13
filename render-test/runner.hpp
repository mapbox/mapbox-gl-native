#pragma once

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>

#include <memory>

struct TestMetadata;

class TestRunner {
public:
    TestRunner() = default;

    bool run(TestMetadata&);
    void reset();

private:
    bool runOperations(const std::string& key, TestMetadata&);
    bool checkImage(mbgl::PremultipliedImage&& image, TestMetadata&);

    struct Impl {
        Impl(const TestMetadata&);

        mbgl::HeadlessFrontend frontend;
        mbgl::Map map;
    };
    std::unordered_map<std::string, std::unique_ptr<Impl>> maps;
};