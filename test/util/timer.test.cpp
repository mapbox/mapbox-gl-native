#include <mbgl/test/util.hpp>

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/timer.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/chrono.hpp>

#include <memory>

using namespace mbgl::util;

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(Basic)) {
    RunLoop loop;

    Timer timer;

    auto callback = [&loop] { loop.stop(); };

    auto interval = mbgl::Milliseconds(300);
    auto expectedTotalTime = interval;

    auto first = mbgl::Clock::now();
    timer.start(interval, mbgl::Duration::zero(), callback);

    loop.run();

    auto totalTime = std::chrono::duration_cast<mbgl::Milliseconds>(mbgl::Clock::now() - first);

    // These are not high precision timers. Especially libuv uses
    // cached time from the beginning of of the main loop iteration
    // and it is very prone to fire earlier, which is, odd.
    EXPECT_GE(totalTime, expectedTotalTime * 0.8);
    EXPECT_LE(totalTime, expectedTotalTime * 1.2);
}

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(Repeat)) {
    RunLoop loop;

    Timer timer;

    unsigned count = 10;
    auto callback = [&] {
        if (!--count) {
            loop.stop();
        }
    };

    auto interval = mbgl::Milliseconds(50);
    auto expectedTotalTime = interval * count;

    auto first = mbgl::Clock::now();
    timer.start(interval, interval, callback);

    loop.run();

    auto totalTime = std::chrono::duration_cast<mbgl::Milliseconds>(mbgl::Clock::now() - first);

    EXPECT_GE(totalTime, expectedTotalTime * 0.8);
    EXPECT_LE(totalTime, expectedTotalTime * 1.2);
}

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(Stop)) {
    RunLoop loop;

    Timer timer1;
    Timer timer2;

    auto interval1 = mbgl::Milliseconds(50);
    auto interval2 = mbgl::Milliseconds(250);
    auto expectedTotalTime = interval2;

    int count = 0;

    auto callback1 = [&] {
        ++count;
        timer1.stop();
    };

    auto callback2 = [&] {
        ++count;
        loop.stop();
    };

    auto first = mbgl::Clock::now();
    timer1.start(interval1, interval1, callback1);
    timer2.start(interval2, mbgl::Duration::zero(), callback2);

    loop.run();

    auto totalTime = std::chrono::duration_cast<mbgl::Milliseconds>(mbgl::Clock::now() - first);

    EXPECT_EQ(count, 2);

    EXPECT_GE(totalTime, expectedTotalTime * 0.8);
    EXPECT_LE(totalTime, expectedTotalTime * 1.2);
}

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(DestroyShouldStop)) {
    RunLoop loop;

    auto timer1 = std::make_unique<Timer>();
    Timer timer2;

    auto interval1 = mbgl::Milliseconds(50);
    auto interval2 = mbgl::Milliseconds(250);
    auto expectedTotalTime = interval2;

    int count = 0;

    auto callback1 = [&] {
        ++count;
        timer1.reset();
    };

    auto callback2 = [&] {
        ++count;
        loop.stop();
    };

    auto first = mbgl::Clock::now();
    timer1->start(interval1, interval1, callback1);
    timer2.start(interval2, mbgl::Duration::zero(), callback2);

    loop.run();

    auto totalTime = std::chrono::duration_cast<mbgl::Milliseconds>(mbgl::Clock::now() - first);

    EXPECT_EQ(count, 2);

    EXPECT_GE(totalTime, expectedTotalTime * 0.8);
    EXPECT_LE(totalTime, expectedTotalTime * 1.2);
}

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(StartOverrides)) {
    RunLoop loop;

    Timer timer;

    auto interval1 = mbgl::Milliseconds(50);
    auto interval2 = mbgl::Milliseconds(250);
    auto expectedTotalTime = interval1  + interval2;

    int count = 0;

    auto callback2 = [&] {
        ++count;
        loop.stop();
    };

    auto callback1 = [&] {
        ++count;
        timer.start(interval2, mbgl::Duration::zero(), callback2);
    };

    auto first = mbgl::Clock::now();
    timer.start(interval1, mbgl::Duration::zero(), callback1);

    loop.run();

    auto totalTime = std::chrono::duration_cast<mbgl::Milliseconds>(mbgl::Clock::now() - first);

    EXPECT_EQ(count, 2);

    EXPECT_GE(totalTime, expectedTotalTime * 0.8);
    EXPECT_LE(totalTime, expectedTotalTime * 1.2);
}

TEST(Timer, TEST_REQUIRES_ACCURATE_TIMING(CanStopNonStartedTimer)) {
    RunLoop loop;

    Timer timer;
    timer.stop();
}
