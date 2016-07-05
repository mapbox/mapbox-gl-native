#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/test/server.hpp>
#include <mbgl/util/thread.hpp>
#include <gtest/gtest.h>

namespace mbgl {

    int runTests(int argc, char *argv[]) {
#if TEST_HAS_SERVER
        std::cout << "Starting server\n";
        util::Thread<test::Server> server({"Webserver"}, true);
        server.invokeSync(&test::Server::start);

        //Make sure the server is up before continueing
        httplib::Client cli("127.0.0.1", 3000);
        int resCode = cli.get("/test")->status;
        while(resCode !=200) {
            std::cout << "No response yet\n";
            usleep(1000*1000);
            resCode = cli.get("/test")->status;
        }
#endif

        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

} // namespace mbgl
