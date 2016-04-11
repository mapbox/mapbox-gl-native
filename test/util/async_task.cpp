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

    EXPECT_EQ(count, 1);
}

TEST(AsyncTask, DestroyShouldNotRunQueue) {
    RunLoop loop;

    unsigned count = 0;
    auto async = std::make_unique<AsyncTask>([&count] { ++count; });

    async->send();
    async.reset();

    EXPECT_EQ(count, 0);
}

TEST(AsyncTask, RequestCoalescingMultithreaded) {
    RunLoop loop;

    unsigned count = 0;
    AsyncTask async([&count] { ++count; });

    std::vector<std::unique_ptr<Thread<TestWorker>>> threads;
    ThreadContext context = {"Test", ThreadType::Map, ThreadPriority::Regular};

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

    EXPECT_EQ(count, 1);
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
    ThreadContext context = {"Test", ThreadType::Map, ThreadPriority::Regular};

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
    EXPECT_GT(count, 0);
}
