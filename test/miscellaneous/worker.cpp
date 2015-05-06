#include "../fixtures/util.hpp"

#include <mbgl/util/worker.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::util;

TEST(Worker, ExecutesWorkAndAfter) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    WorkRequest request;

    bool didWork = false;
    bool didAfter = false;

    loop.invoke([&] {
        request = worker.send([&] {
            didWork = true;
        }, [&] {
            didAfter = true;
            loop.stop();
        });
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    EXPECT_TRUE(didWork);
    EXPECT_TRUE(didAfter);
}

TEST(Worker, WorkRequestJoinWaitsForWorkToComplete) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    bool didWork = false;

    loop.invoke([&] {
        WorkRequest request = worker.send([&] {
            usleep(10000);
            didWork = true;
        }, [&] {});

        request.join();
        EXPECT_TRUE(didWork);
        loop.stop();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(Worker, WorkRequestJoinCancelsAfter) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    bool didAfter = false;

    loop.invoke([&] {
        WorkRequest request = worker.send([&] {
        }, [&] {
            didAfter = true;
        });

        request.join();
        loop.stop();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    EXPECT_FALSE(didAfter);
}
