#include <mbgl/test/util.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_queue.hpp>

#include <thread>

using namespace mbgl::util;

class TestThread {
public:
    TestThread(WorkQueue* queue_) : queue(queue_) {}

    void send(std::function<void()>&& fn) {
        queue->push(std::move(fn));
    }

private:
    WorkQueue* queue;
};

TEST(WorkQueue, push) {
    RunLoop loop;

    WorkQueue queue;
    Thread<TestThread> thread({"Test"}, &queue);

    uint8_t count = 0;

    auto endTest = [&]() {
        if (++count == 4) {
            loop.stop();
        }
    };

    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);
    thread.invoke(&TestThread::send, endTest);

    loop.run();
}

TEST(WorkQueue, cancel) {
    RunLoop loop;

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
