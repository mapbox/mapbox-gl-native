#include <mbgl/util/async_task.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>

#include <mbgl/test/util.hpp>

#include <vector>

using namespace mbgl::util;

namespace {

class TestWorker {
public:
    TestWorker(AsyncTask *async_)
        : async(async_) {}

    void run() {
        for (unsigned i = 0; i < 100000; ++i) {
            async->send();
        }
    }

    void runWithCallback(std::function<void()> cb) {
        for (unsigned i = 0; i < 100000; ++i) {
            async->send();
        }

        cb();
    }

private:
    AsyncTask *async;
};

} // namespace

TEST(AsyncTask, RequestCoalescing) {
    RunLoop loop;

    unsigned count = 0;
    AsyncTask async([&count] { ++count; });

    async.send();
    async.send();
    async.send();
    async.send();
    async.send();

    loop.runOnce();

    EXPECT_EQ(count, 1u);
}

TEST(AsyncTask, DestroyShouldNotRunQueue) {
    RunLoop loop;

    unsigned count = 0;
    auto async = std::make_unique<AsyncTask>([&count] { ++count; });

    async->send();
    async.reset();

    EXPECT_EQ(count, 0u);
}

TEST(AsyncTask, DestroyAfterSignaling) {
    RunLoop loop;

    // We're creating two tasks and signal both of them; the one that gets fired first destroys
    // the other one. Make sure that the second one we destroyed doesn't fire.

    std::unique_ptr<AsyncTask> task1, task2;

    task1 = std::make_unique<AsyncTask>([&] {
        task2.reset();
        if (!task1) {
            FAIL() << "Task was destroyed but invoked anyway";
        }
    });
    task2 = std::make_unique<AsyncTask>([&] {
        task1.reset();
        if (!task2) {
            FAIL() << "Task was destroyed but invoked anyway";
        }
    });

    task1->send();
    task2->send();

    loop.runOnce();
}

TEST(AsyncTask, RequestCoalescingMultithreaded) {
    RunLoop loop;

    unsigned count = 0;
    AsyncTask async([&count] { ++count; });

    std::vector<std::unique_ptr<Thread<TestWorker>>> threads;
    ThreadContext context = {"Test"};

    unsigned numThreads = 25;
    for (unsigned i = 0; i < numThreads; ++i) {
        std::unique_ptr<Thread<TestWorker>> thread =
            std::make_unique<Thread<TestWorker>>(context, &async);

        thread->invoke(&TestWorker::run);
        threads.push_back(std::move(thread));
    }

    // Join all the threads
    threads.clear();

    loop.runOnce();

    EXPECT_EQ(count, 1u);
}

TEST(AsyncTask, ThreadSafety) {
    RunLoop loop;

    unsigned count = 0;
    AsyncTask async([&count] { ++count; });

    unsigned numThreads = 25;

    auto callback = [&] {
        if (!--numThreads) {
            loop.stop();
        }
    };

    std::vector<std::unique_ptr<Thread<TestWorker>>> threads;
    std::vector<std::unique_ptr<mbgl::AsyncRequest>> requests;
    ThreadContext context = {"Test"};

    for (unsigned i = 0; i < numThreads; ++i) {
        std::unique_ptr<Thread<TestWorker>> thread =
            std::make_unique<Thread<TestWorker>>(context, &async);

        requests.push_back(
            thread->invokeWithCallback(&TestWorker::runWithCallback, callback));

        threads.push_back(std::move(thread));
    }

    loop.run();

    // We expect here more than 1 but 1 would also be
    // a valid result, although very unlikely (I hope).
    EXPECT_GT(count, 0u);
}
