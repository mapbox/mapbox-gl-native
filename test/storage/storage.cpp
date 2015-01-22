#include "storage.hpp"

pid_t Storage::pid = 0;

void Storage::SetUpTestCase() {
    const auto server = mbgl::test::getBaseDirectory() + "/storage/server.js";
    fprintf(stderr, "server: %s\n", server.c_str());
    pid = mbgl::test::startServer(server.c_str());
}

void Storage::TearDownTestCase() {
    mbgl::test::stopServer(pid);
}