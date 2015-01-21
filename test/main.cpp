#include "util.hpp"
#include "fixtures/server_environment.hpp"

GTEST_API_ int main(int argc, char *argv[]) {
    testing::AddGlobalTestEnvironment(new ServerEnvironment("test/headless/server.js"));
    testing::AddGlobalTestEnvironment(new ServerEnvironment("test/storage/server.js"));

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
