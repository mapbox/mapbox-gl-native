#include "storage.hpp"

#include <mbgl/platform/platform.hpp>

std::unique_ptr<mbgl::test::Server> Storage::server;

void Storage::SetUpTestCase() {
    const auto program = mbgl::platform::applicationRoot() + "/TEST_DATA/storage/server.js";
    server = std::make_unique<mbgl::test::Server>(program.c_str());
}

void Storage::TearDownTestCase() {
    server.reset();
}
