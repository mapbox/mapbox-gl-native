#include "../fixtures/util.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_queue.hpp>

#include <thread>

using namespace mbgl::util;

class TestThread {
public:
    TestThread(WorkQueue* queue_) : queue(queue_) {}

    void send(std::function<void()>&& fn) {
        EXPECT_TRUE(ThreadContext::currentlyOn(ThreadType::Map));

        queue->push(std::move(fn));
    }

private:
    WorkQueue* queue;
};

TEST(WorkQueue, push) {
    RunLoop loop(uv_default_loop());

    WorkQueue queue;
    Thread<TestThread> thread({"Test", ThreadType::Map, ThreadPriority::Regular}, &queue);

    uint8_t count = 0;

    auto endTest = [&]() {
        EXPECT_TRUE(ThreadContext::currentlyOn(ThreadType::Main));

        if (++count == 4) {
            loop.stop();
        }
    };

    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(WorkQueue, cancel) {
    RunLoop loop(uv_default_loop());

    WorkQueue queue;

    auto work = [&]() {
        FAIL() << "Should never be called";
    };

    queue.push(work);
    queue.push(work);
    queue.push(work);
    queue.push(work);
    queue.push(work);
}
