#include "storage.hpp"

std::unique_ptr<mbgl::test::Server> Storage::server;

void Storage::SetUpTestCase() {
#if TEST_HAS_SERVER
    server = std::make_unique<mbgl::test::Server>("test/storage/server.js");
#endif
}

void Storage::TearDownTestCase() {
    server.reset();
}
