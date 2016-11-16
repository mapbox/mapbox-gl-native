#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/test/server.hpp>

#include <mbgl/util/thread.hpp>
#include <mbgl/platform/log.hpp>

#include <gtest/gtest.h>

namespace mbgl {

    int runTests(int argc, char *argv[]) {
#if TEST_HAS_SERVER
        Log::Info(Event::General, "Starting server");
        util::Thread<test::Server> server({"Webserver"}, true);
        server.invokeSync(&test::Server::start);

        //Make sure the server is up before continueing
        httplib::Client client("127.0.0.1", 3000);
        while (client.get("/test")->status != 200) {
            Log::Info(Event::General, "No response yet, waiting for mock webserver");
            usleep(1000 * 1000);
        }
#endif

        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

} // namespace mbgl
