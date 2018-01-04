#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/timer.hpp>

#include <atomic>
#include <memory>

using namespace mbgl;
using namespace mbgl::util;

class TestObject {
public:
    TestObject(ActorRef<TestObject>, std::thread::id otherTid)
        : tid(std::this_thread::get_id()) {
        EXPECT_NE(tid, otherTid);
    }

    ~TestObject() {
        EXPECT_EQ(tid, std::this_thread::get_id());
    }

    void fn1(int val) const {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(val, 1);
    }

    void fn2(std::function<void (int)> cb) const {
        EXPECT_EQ(tid, std::this_thread::get_id());
        cb(1);
    }

    void transferIn(std::unique_ptr<int> val) const {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    void transferInShared(std::shared_ptr<int> val) const {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    void transferString(const std::string& string) const {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(string, "test");
    }

    void checkContext(std::promise<bool> result) const {
        result.set_value(tid == std::this_thread::get_id());
    }

    void sync(std::promise<void> result) const {
        result.set_value();
    }

    const std::thread::id tid;
};

TEST(Thread, invoke) {
    const std::thread::id tid = std::this_thread::get_id();
    Thread<TestObject> thread("Test", tid);

    thread.actor().invoke(&TestObject::fn1, 1);
    thread.actor().invoke(&TestObject::fn2, [] (int result) { EXPECT_EQ(result, 1); } );
    thread.actor().invoke(&TestObject::transferIn, std::make_unique<int>(1));
    thread.actor().invoke(&TestObject::transferInShared, std::make_shared<int>(1));

    std::string test("test");
    thread.actor().invoke(&TestObject::transferString, test);

    // Make sure the message queue was consumed before ending the test.
    std::promise<void> result;
    auto resultFuture = result.get_future();
    thread.actor().invoke(&TestObject::sync, std::move(result));
    resultFuture.get();
}

TEST(Thread, Context) {
    const std::thread::id tid = std::this_thread::get_id();
    Thread<TestObject> thread("Test", tid);

    std::promise<bool> result;
    auto resultFuture = result.get_future();

    thread.actor().invoke(&TestObject::checkContext, std::move(result));
    EXPECT_EQ(resultFuture.get(), true);
}

class TestWorker {
public:
    TestWorker(ActorRef<TestWorker>) {}

    void send(std::function<void ()> cb) {
        cb();
    }

    void sendDelayed(std::function<void ()> cb) {
        timer.start(Milliseconds(300), mbgl::Duration::zero(), [cb] {
            cb();
        });
    }

private:
    Timer timer;
};

TEST(Thread, ExecutesAfter) {
    RunLoop loop;
    Thread<TestWorker> thread("Test");

    bool didWork = false;
    bool didAfter = false;

    thread.actor().invoke(&TestWorker::send, [&] { didWork = true; });
    thread.actor().invoke(&TestWorker::send, [&] { didAfter = true; loop.stop(); });

    loop.run();

    EXPECT_TRUE(didWork);
    EXPECT_TRUE(didAfter);
}

TEST(Thread, CanSelfWakeUp) {
    RunLoop loop;
    Thread<TestWorker> thread("Test");

    thread.actor().invoke(&TestWorker::sendDelayed, [&] {
        loop.stop();
    });

    loop.run();
}

TEST(Thread, Concurrency) {
    auto loop = std::make_shared<RunLoop>();

    unsigned numMessages = 100000;
    std::atomic_uint completed(numMessages);

    ThreadPool threadPool(10);
    Actor<TestWorker> poolWorker(threadPool);
    auto poolWorkerRef = poolWorker.self();

    Thread<TestWorker> threadedObject("Test");
    auto threadedObjectRef = threadedObject.actor();

    // 10 threads sending 100k messages to the Thread. The
    // idea here is to test if the scheduler is handling concurrency
    // correctly, otherwise this test should crash.
    for (unsigned i = 0; i < numMessages; ++i) {
        poolWorkerRef.invoke(&TestWorker::send, [threadedObjectRef, loop, &completed] () mutable {
            threadedObjectRef.invoke(&TestWorker::send, [loop, &completed] () {
                if (!--completed) {
                    loop->stop();
                }
            });
        });
    };

    loop->run();
}

TEST(Thread, ThreadPoolMessaging) {
    auto loop = std::make_shared<RunLoop>();

    ThreadPool threadPool(1);
    Actor<TestWorker> poolWorker(threadPool);
    auto poolWorkerRef = poolWorker.self();

    Thread<TestWorker> threadedObject("Test");
    auto threadedObjectRef = threadedObject.actor();

    // This is sending a message to the Thread from the main
    // thread. Then the Thread will send another message to
    // a worker on the ThreadPool.
    threadedObjectRef.invoke(&TestWorker::send, [poolWorkerRef, loop] () mutable {
        poolWorkerRef.invoke(&TestWorker::send, [loop] () { loop->stop(); });
    });

    loop->run();

    // Same as before, but in the opposite direction.
    poolWorkerRef.invoke(&TestWorker::send, [threadedObjectRef, loop] () mutable {
        threadedObjectRef.invoke(&TestWorker::send, [loop] () { loop->stop(); });
    });

    loop->run();
}

TEST(Thread, ReferenceCanOutliveThread) {
    auto thread = std::make_unique<Thread<TestWorker>>("Test");
    auto worker = thread->actor();

    thread.reset();

    for (unsigned i = 0; i < 1000; ++i) {
        worker.invoke(&TestWorker::send, [&] { ADD_FAILURE() << "Should never happen"; });
    }

    usleep(10000);
}

TEST(Thread, DeletePausedThread) {
    std::atomic_bool flag(false);

    auto thread = std::make_unique<Thread<TestWorker>>("Test");
    thread->pause();
    thread->actor().invoke(&TestWorker::send, [&] { flag = true; });

    // Should not hang.
    thread.reset();

    // Should process the queue before destruction.
    ASSERT_TRUE(flag);
}

TEST(Thread, Pause) {
    RunLoop loop;

    std::atomic_bool flag(false);

    Thread<TestWorker> thread1("Test1");
    thread1.pause();

    Thread<TestWorker> thread2("Test2");

    for (unsigned i = 0; i < 100; ++i) {
        thread1.actor().invoke(&TestWorker::send, [&] { flag = true; });
        thread2.actor().invoke(&TestWorker::send, [&] { ASSERT_FALSE(flag); });
    }

    // Queue a message at the end of thread2 queue.
    thread2.actor().invoke(&TestWorker::send, [&] { loop.stop(); });
    loop.run();
}

TEST(Thread, Resume) {
    RunLoop loop;

    std::atomic_bool flag(false);

    Thread<TestWorker> thread("Test");
    thread.pause();

    for (unsigned i = 0; i < 100; ++i) {
        thread.actor().invoke(&TestWorker::send, [&] { flag = true; });
    }

    // Thread messages are ondered, when we resume, this is going
    // to me the last thing to run on the message queue.
    thread.actor().invoke(&TestWorker::send, [&] { loop.stop(); });

    // This test will be flaky if the thread doesn't get paused.
    ASSERT_FALSE(flag);

    thread.resume();
    loop.run();

    ASSERT_TRUE(flag);
}

TEST(Thread, PauseResume) {
    RunLoop loop;

    Thread<TestWorker> thread("Test");

    // Test if multiple pause/resume work.
    for (unsigned i = 0; i < 100; ++i) {
        thread.pause();
        thread.resume();
    }

    thread.actor().invoke(&TestWorker::send, [&] { loop.stop(); });
    loop.run();
}
