#include <mbgl/util/thread.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/test/util.hpp>

using namespace mbgl::util;

class TestObject {
public:
    TestObject(std::thread::id otherTid)
        : tid(std::this_thread::get_id()) {
        EXPECT_NE(tid, otherTid);
    }

    void fn1(int val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(val, 1);
    }

    void fn2(std::function<void (int)> cb) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        cb(1);
    }

    void transferIn(std::unique_ptr<int> val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    void transferOut(std::function<void (std::unique_ptr<int>)> cb) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        cb(std::make_unique<int>(1));
    }

    void transferInOut(std::unique_ptr<int> val, std::function<void (std::unique_ptr<int>)> cb) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
        cb(std::move(val));
    }

    void transferInShared(std::shared_ptr<int> val) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(*val, 1);
    }

    void transferOutShared(std::function<void (std::shared_ptr<int>)> cb) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        cb(std::make_shared<int>(1));
    }

    void transferString(const std::string& string, std::function<void (std::string)> cb) {
        EXPECT_EQ(tid, std::this_thread::get_id());
        EXPECT_EQ(string, "test");
        cb(string);
    }

    void checkContext(std::function<void (bool)> cb) const {
        cb(tid == std::this_thread::get_id());
    }

    const std::thread::id tid;
};

TEST(Thread, invoke) {
    const std::thread::id tid = std::this_thread::get_id();

    RunLoop loop;
    std::vector<std::unique_ptr<mbgl::AsyncRequest>> requests;

    loop.invoke([&] {
        EXPECT_EQ(tid, std::this_thread::get_id());
        Thread<TestObject> thread({"Test"}, tid);

        thread.invoke(&TestObject::fn1, 1);
        requests.push_back(thread.invokeWithCallback(&TestObject::fn2, [&] (int result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(result, 1);
        }));

        thread.invoke(&TestObject::transferIn, std::make_unique<int>(1));
        requests.push_back(thread.invokeWithCallback(&TestObject::transferOut, [&] (std::unique_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }));

        requests.push_back(thread.invokeWithCallback(&TestObject::transferInOut, std::make_unique<int>(1), [&] (std::unique_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }));

        thread.invoke(&TestObject::transferInShared, std::make_shared<int>(1));
        requests.push_back(thread.invokeWithCallback(&TestObject::transferOutShared, [&] (std::shared_ptr<int> result) {
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(*result, 1);
        }));

        // Cancelled request
        thread.invokeWithCallback(&TestObject::fn2, [&] (int) {
            ADD_FAILURE();
        });

        std::string test("test");
        requests.push_back(thread.invokeWithCallback(&TestObject::transferString, test, [&] (std::string result){
            EXPECT_EQ(tid, std::this_thread::get_id());
            EXPECT_EQ(result, "test");
            loop.stop();
        }));
        test.clear();
    });

    loop.run();
}

TEST(Thread, context) {
    const std::thread::id tid = std::this_thread::get_id();

    RunLoop loop;
    std::vector<std::unique_ptr<mbgl::AsyncRequest>> requests;

    loop.invoke([&] {
        Thread<TestObject> thread({"Test"}, tid);

        requests.push_back(thread.invokeWithCallback(&TestObject::checkContext, [&] (bool inTestThreadContext) {
            EXPECT_EQ(inTestThreadContext, true);
            loop.stop();
        }));
    });

    loop.run();
}

class TestWorker {
public:
    TestWorker() = default;

    void send(std::function<void ()> fn, std::function<void ()> cb) {
        fn();
        cb();
    }
};

TEST(Thread, ExecutesAfter) {
    RunLoop loop;
    Thread<TestWorker> thread({"Test"});

    bool didWork = false;
    bool didAfter = false;

    auto request = thread.invokeWithCallback(&TestWorker::send, [&] {
        didWork = true;
    }, [&] {
        didAfter = true;
        loop.stop();
    });

    loop.run();

    EXPECT_TRUE(didWork);
    EXPECT_TRUE(didAfter);
}

TEST(Thread, WorkRequestDeletionWaitsForWorkToComplete) {
    RunLoop loop;

    Thread<TestWorker> thread({"Test"});

    std::promise<void> started;
    bool didWork = false;

    auto request = thread.invokeWithCallback(&TestWorker::send, [&] {
        started.set_value();
        usleep(10000);
        didWork = true;
    }, [&] {});

    started.get_future().get();
    request.reset();
    EXPECT_TRUE(didWork);
}

TEST(Thread, WorkRequestDeletionCancelsAfter) {
    RunLoop loop;
    Thread<TestWorker> thread({"Test"});

    std::promise<void> started;
    bool didAfter = false;

    auto request = thread.invokeWithCallback(&TestWorker::send, [&] {
        started.set_value();
    }, [&] {
        didAfter = true;
    });

    started.get_future().get();
    request.reset();
    loop.runOnce();
    EXPECT_FALSE(didAfter);
}

TEST(Thread, WorkRequestDeletionCancelsImmediately) {
    RunLoop loop;
    Thread<TestWorker> thread({"Test"});

    std::promise<void> started;

    auto request1 = thread.invokeWithCallback(&TestWorker::send, [&] {
        usleep(10000);
        started.set_value();
    }, [&] {});

    auto request2 = thread.invokeWithCallback(&TestWorker::send, [&] {
        ADD_FAILURE() << "Second work item should not be invoked";
    }, [&] {});
    request2.reset();

    started.get_future().get();
    request1.reset();
}
