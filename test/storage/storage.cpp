#include "storage.hpp"

#include <mbgl/platform/platform.hpp>

pid_t Storage::pid = 0;

void Storage::SetUpTestCase() {
    const auto server = mbgl::platform::applicationRoot() + "/TEST_DATA/storage/server.js";
    pid = mbgl::test::startServer(server.c_str());
}

void Storage::TearDownTestCase() {
    mbgl::test::stopServer(pid);
}