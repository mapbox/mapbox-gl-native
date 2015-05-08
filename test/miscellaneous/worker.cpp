#include "../fixtures/util.hpp"

#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/run_loop.hpp>

using namespace mbgl;
using namespace mbgl::util;

TEST(Worker, ExecutesWorkAndAfter) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    std::unique_ptr<WorkRequest> request;

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

TEST(Worker, WorkRequestDeletionWaitsForWorkToComplete) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    std::promise<void> started;
    bool didWork = false;

    loop.invoke([&] {
        auto request = worker.send([&] {
            started.set_value();
            usleep(10000);
            didWork = true;
        }, [&] {});
        started.get_future().get();

        request.reset();
        EXPECT_TRUE(didWork);
        loop.stop();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(Worker, WorkRequestJoinCancelsAfter) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);
    std::promise<void> started;
    bool didAfter = false;

    loop.invoke([&] {
        auto request = worker.send([&] {
            started.set_value();
        }, [&] {
            didAfter = true;
        });
        started.get_future().get();

        request.reset();
        loop.stop();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    EXPECT_FALSE(didAfter);
}

TEST(Worker, WorkRequestCancelsImmediately) {
    RunLoop loop(uv_default_loop());

    Worker worker(1);

    loop.invoke([&] {
        std::promise<void> started;
        // First worker item.
        auto request1 = worker.send([&] {
            usleep(10000);
            started.set_value();
        }, [&] {});

        auto request2 = worker.send([&] {
            ADD_FAILURE() << "Second work item should not be invoked";
        }, [&] {});
        request2.reset();

        started.get_future().get();
        request1.reset();

        loop.stop();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
