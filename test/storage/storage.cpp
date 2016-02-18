#include "storage.hpp"

std::unique_ptr<mbgl::test::Server> Storage::server;

void Storage::SetUpTestCase() {
    server = std::make_unique<mbgl::test::Server>("test/storage/server.js");
}

void Storage::TearDownTestCase() {
    server.reset();
}
