#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <mbgl/test/util.hpp>

#include <atomic>
#include <condition_variable>
#include <thread>

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

TEST(RunLoop, PlatformIntegration) {
    std::atomic<int> count1(0);

    // No need to be atomic, it will run
    // on the main thread.
    int count2(0);

    std::thread::id mainThread = std::this_thread::get_id();

    RunLoop loop;

    std::mutex mutex;
    std::condition_variable cv;

    loop.setPlatformCallback([&] {
        EXPECT_NE(mainThread, std::this_thread::get_id());
        count1++;
        cv.notify_one();
    });

    auto threadBody = [&]() {
        for (unsigned i = 0; i < 100000; ++i) {
            loop.invoke([&] {
                EXPECT_EQ(mainThread, std::this_thread::get_id());
                count2++;
            });
        }
    };

    std::thread thread1(threadBody);
    std::thread thread2(threadBody);

    while (count2 < 200000) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock);
        loop.runOnce();
    }

    EXPECT_EQ(count1, 200000);
    EXPECT_EQ(count2, 200000);

    thread1.join();
    thread2.join();
}
