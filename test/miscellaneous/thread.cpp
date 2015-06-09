#include <mbgl/util/thread.hpp>
#include <mbgl/util/run_loop.hpp>

#include "../fixtures/util.hpp"

using namespace mbgl::util;

class TestObject {
public:
    TestObject(uv_loop_t*, std::thread::id otherTid)
        : tid(std::this_thread::get_id()) {
        EXPECT_NE(tid, otherTid);
    }

    void fn1(int val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(val, 1);
    }

    int fn2() {
        EXPECT_EQ(tid, std::this_thread::get_id());
        return 1;
    }

    void transferIn(std::unique_ptr<int> val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    std::unique_ptr<int> transferOut() {
        EXPECT_EQ(tid, std::this_thread::get_id());
        return std::make_unique<int>(1);
    }

    std::unique_ptr<int> transferInOut(std::unique_ptr<int> val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
        return std::move(val);
    }

    void transferInShared(std::shared_ptr<int> val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    std::shared_ptr<int> transferOutShared() {
        EXPECT_EQ(tid, std::this_thread::get_id());
        return std::make_shared<int>(1);
    }

    std::string transferString(const std::string& string) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(string, "test");
        return string;
    }

    bool checkContext() const {
        return ThreadContext::currentlyOn(ThreadType::Worker)
            && ThreadContext::getName() == "Test"
            && ThreadContext::getPriority() == ThreadPriority::Low;
    }

    const std::thread::id tid;
};

TEST(Thread, invoke) {
    const std::thread::id tid = std::this_thread::get_id();

    RunLoop loop(uv_default_loop());
    std::vector<std::unique_ptr<mbgl::WorkRequest>> requests;

    loop.invoke([&] {
        EXPECT_EQ(tid, std::this_thread::get_id());
        Thread<TestObject> thread({"Test", ThreadType::Map, ThreadPriority::Regular}, tid);

        thread.invoke(&TestObject::fn1, 1);
        requests.push_back(thread.invokeWithResult<int>(&TestObject::fn2, [&] (int result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(result, 1);
        }));

        thread.invoke(&TestObject::transferIn, std::make_unique<int>(1));
        requests.push_back(thread.invokeWithResult<std::unique_ptr<int>>(&TestObject::transferOut, [&] (std::unique_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }));

        requests.push_back(thread.invokeWithResult<std::unique_ptr<int>>(&TestObject::transferInOut, [&] (std::unique_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }, std::make_unique<int>(1)));

        thread.invoke(&TestObject::transferInShared, std::make_shared<int>(1));
        requests.push_back(thread.invokeWithResult<std::shared_ptr<int>>(&TestObject::transferOutShared, [&] (std::shared_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }));

        // Cancelled request
        thread.invokeWithResult(&TestObject::fn1, [&] {
            ADD_FAILURE();
        }, 1);

        std::string test("test");
        requests.push_back(thread.invokeWithResult<std::string>(&TestObject::transferString, [&] (std::string result){
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(result, "test");
            loop.stop();
        }, test));
        test.clear();
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

TEST(Thread, context) {
    bool isMainThreadContext = ThreadContext::currentlyOn(ThreadType::Main)
        && ThreadContext::getName() == "Main"
        && ThreadContext::getPriority() == ThreadPriority::Regular;

    EXPECT_EQ(isMainThreadContext, true);

    const std::thread::id tid = std::this_thread::get_id();

    RunLoop loop(uv_default_loop());
    std::vector<std::unique_ptr<mbgl::WorkRequest>> requests;

    loop.invoke([&] {
        Thread<TestObject> thread({"Test", ThreadType::Worker, ThreadPriority::Low}, tid);

        requests.push_back(thread.invokeWithResult<bool>(&TestObject::checkContext, [&] (bool inTestThreadContext) {
            EXPECT_EQ(inTestThreadContext, true);
            loop.stop();
        }));
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

class TestWorker {
public:
    TestWorker(uv_loop_t*) {}
    void send(std::function<void ()> fn) { fn(); }
};

TEST(Thread, ExecutesAfter) {
    RunLoop loop(uv_default_loop());
    Thread<TestWorker> thread({"Test", ThreadType::Map, ThreadPriority::Regular});

    bool didWork = false;
    bool didAfter = false;

    auto request = thread.invokeWithResult(&TestWorker::send, [&] {
        didAfter = true;
        loop.stop();
    }, [&] {
        didWork = true;
    });

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    EXPECT_TRUE(didWork);
    EXPECT_TRUE(didAfter);
}

TEST(Thread, WorkRequestDeletionWaitsForWorkToComplete) {
    RunLoop loop(uv_default_loop());
    Thread<TestWorker> thread({"Test", ThreadType::Map, ThreadPriority::Regular});

    std::promise<void> started;
    bool didWork = false;

    auto request = thread.invokeWithResult(&TestWorker::send, [&] {}, [&] {
        started.set_value();
        usleep(10000);
        didWork = true;
    });

    started.get_future().get();
    request.reset();
    EXPECT_TRUE(didWork);
}

TEST(Thread, WorkRequestDeletionCancelsAfter) {
    RunLoop loop(uv_default_loop());
    Thread<TestWorker> thread({"Test", ThreadType::Map, ThreadPriority::Regular});

    std::promise<void> started;
    bool didAfter = false;

    auto request = thread.invokeWithResult(&TestWorker::send, [&] {
        didAfter = true;
    }, [&] {
        started.set_value();
    });

    started.get_future().get();
    request.reset();
    uv_run(uv_default_loop(), UV_RUN_ONCE);
    EXPECT_FALSE(didAfter);
}

TEST(Thread, WorkRequestDeletionCancelsImmediately) {
    RunLoop loop(uv_default_loop());
    Thread<TestWorker> thread({"Test", ThreadType::Map, ThreadPriority::Regular});

    std::promise<void> started;

    auto request1 = thread.invokeWithResult(&TestWorker::send, [&] {}, [&] {
        usleep(10000);
        started.set_value();
    });

    auto request2 = thread.invokeWithResult(&TestWorker::send, [&] {}, [&] {
        ADD_FAILURE() << "Second work item should not be invoked";
    });
    request2.reset();

    started.get_future().get();
    request1.reset();
}
