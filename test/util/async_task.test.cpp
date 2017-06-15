#include <mbgl/util/async_task.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/actor/actor_ref.hpp>

#include <mbgl/test/util.hpp>

#include <atomic>
#include <future>
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

    void sync(std::promise<void> barrier) {
        barrier.set_value();
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

    unsigned count = 0, numThreads = 25;
    AsyncTask async([&count] { ++count; });

    mbgl::ThreadPool threads(numThreads);
    auto mailbox = std::make_shared<mbgl::Mailbox>(threads);

    TestWorker worker(&async);
    mbgl::ActorRef<TestWorker> workerRef(worker, mailbox);

    for (unsigned i = 0; i < numThreads; ++i) {
        workerRef.invoke(&TestWorker::run);
    }

    std::promise<void> barrier;
    std::future<void> barrierFuture = barrier.get_future();

    workerRef.invoke(&TestWorker::sync, std::move(barrier));
    barrierFuture.wait();

    loop.runOnce();

    EXPECT_EQ(count, 1u);
}

TEST(AsyncTask, ThreadSafety) {
    RunLoop loop;

    unsigned count = 0, numThreads = 25;
    std::atomic_uint completed(numThreads);

    AsyncTask async([&count] { ++count; });

    mbgl::ThreadPool threads(numThreads);
    auto mailbox = std::make_shared<mbgl::Mailbox>(threads);

    TestWorker worker(&async);
    mbgl::ActorRef<TestWorker> workerRef(worker, mailbox);

    for (unsigned i = 0; i < numThreads; ++i) {
        // The callback runs on the worker, thus the atomic type.
        workerRef.invoke(&TestWorker::runWithCallback, [&] { if (!--completed) loop.stop(); });
    }

    loop.run();

    // We expect here more than 1 but 1 would also be
    // a valid result, although very unlikely (I hope).
    EXPECT_GT(count, 0u);
}
