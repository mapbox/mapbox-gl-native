#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <mbgl/test/util.hpp>

using namespace mbgl::util;

TEST(RunLoop, Stop) { // NOLINT
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
    });

    loop.run();
}

TEST(RunLoop, MultipleStop) { // NOLINT
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
        loop.stop();
        loop.stop();
        loop.stop();
    });

    loop.run();
}

TEST(RunLoop, MultipleRun) { // NOLINT
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
    });

    loop.run();

    bool secondTimeout = false;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        secondTimeout = true;
        loop.stop();
    });

    loop.run();

    EXPECT_TRUE(secondTimeout);
}
