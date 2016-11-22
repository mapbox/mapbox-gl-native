#include <mbgl/actor/actor.hpp>
#include <mbgl/util/default_thread_pool.hpp>

#include <mbgl/test/util.hpp>

#include <chrono>
#include <functional>
#include <future>

using namespace mbgl;
using namespace std::chrono_literals;

TEST(Actor, Construction) {
    // Construction is currently synchronous. It may become asynchronous in the future.

    struct Test {
        Test(ActorRef<Test>, bool& constructed) {
            constructed = true;
        };
    };

    ThreadPool pool { 1 };
    bool constructed = false;
    Actor<Test> test(pool, std::ref(constructed));

    EXPECT_TRUE(constructed);
}

TEST(Actor, DestructionClosesMailbox) {
    // Destruction blocks until the actor is not receiving.

    struct Test {
        std::promise<void> promise;
        std::future<void> future;
        std::atomic<bool> waited;

        Test(ActorRef<Test>, std::promise<void> promise_, std::future<void> future_)
            : promise(std::move(promise_)),
              future(std::move(future_)),
              waited(false) {
        }

        ~Test() {
            EXPECT_TRUE(waited.load());
        }

        void wait() {
            promise.set_value();
            future.wait();
            std::this_thread::sleep_for(1ms);
            waited = true;
        }
    };

    ThreadPool pool { 1 };

    std::promise<void> enteredPromise;
    std::future<void> enteredFuture = enteredPromise.get_future();

    std::promise<void> exitingPromise;
    std::future<void> exitingFuture = exitingPromise.get_future();

    Actor<Test> test(pool, std::move(enteredPromise), std::move(exitingFuture));

    test.invoke(&Test::wait);
    enteredFuture.wait();
    exitingPromise.set_value();
}

TEST(Actor, OrderedMailbox) {
    // Messages are processed in order.

    struct Test {
        int last = 0;
        std::promise<void> promise;

        Test(ActorRef<Test>, std::promise<void> promise_)
            : promise(std::move(promise_))  {
        }

        void receive(int i) {
            EXPECT_EQ(i, last + 1);
            last = i;
        }

        void end() {
            promise.set_value();
        }
    };

    ThreadPool pool { 1 };

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(pool, std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.invoke(&Test::receive, i);
    }

    test.invoke(&Test::end);
    endedFuture.wait();
}

TEST(Actor, NonConcurrentMailbox) {
    // An individual actor is never itself concurrent.

    struct Test {
        int last = 0;
        std::promise<void> promise;

        Test(ActorRef<Test>, std::promise<void> promise_)
            : promise(std::move(promise_))  {
        }

        void receive(int i) {
            EXPECT_EQ(i, last + 1);
            last = i;
            std::this_thread::sleep_for(1ms);
        }

        void end() {
            promise.set_value();
        }
    };

    ThreadPool pool { 10 };

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(pool, std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.invoke(&Test::receive, i);
    }

    test.invoke(&Test::end);
    endedFuture.wait();
}
