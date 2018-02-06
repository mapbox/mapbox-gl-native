#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <mbgl/test/util.hpp>

using namespace mbgl::util;

TEST(RunLoop, Stop) {
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
    });

    loop.run();
}

TEST(RunLoop, MultipleStop) {
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

TEST(RunLoop, MultipleRun) {
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

TEST(RunLoop, Priorities) {
    std::vector<int> order;

    RunLoop loop(RunLoop::Type::New);
    loop.invoke([&] { order.push_back(1); });
    loop.invoke(RunLoop::Priority::High, [&] { order.push_back(2); });
    loop.invoke([&] { order.push_back(3); });
    loop.invoke(RunLoop::Priority::High, [&] { order.push_back(4); });
    loop.invoke(RunLoop::Priority::Default, [&] { loop.stop(); });
    loop.run();

    EXPECT_EQ((std::vector<int>{ 2, 4, 1, 3 }), order);
}
