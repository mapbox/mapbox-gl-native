#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/thread.hpp>
#include <gtest/gtest.h>

namespace mbgl {

int runTests(int argc, char *argv[]) {
#if TEST_HAS_SERVER
    std::cout << "Starting server\n";
    util::Thread<test::Server> server({"Webserver"}, true);
    server.invokeSync(&test::Server::start);

    //Sleep so we know the server actually started
    usleep(5 * 1000 * 1000);
#endif

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

} // namespace mbgl
