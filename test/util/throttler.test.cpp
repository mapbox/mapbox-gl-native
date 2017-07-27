#include <mbgl/actor/actor.hpp>
#include <mbgl/util/throttler.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/test/util.hpp>

#include <chrono>
#include <functional>
#include <future>
#include <memory>

using namespace mbgl;
using namespace mbgl::util;
using namespace std::chrono_literals;


TEST(Throttler, Basic) {
    // Ensure the delegate is called
    
    // Throttler needs a RunLoop
    RunLoop loop;
    
    struct Test {
        std::promise<void> promise;
        
        Test(ActorRef<Test>, std::promise<void> promise_) : promise(std::move(promise_)) {}
        
        void doIt() {
            promise.set_value();
        }
    };
    
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    ThreadPool pool { 2 };
    Actor<Test> test(pool, std::move(promise));
    
    Throttler<Test> throttler { test.self(), Milliseconds { 300 } };
    
    throttler.invoke(&Test::doIt);
    
    future.wait();
    
}

TEST(Throttler, Throttling) {
    // Ensure invocations are throttled
    
    RunLoop loop;
    
    static Milliseconds frequency { 200 };
    
    struct Test {
        TimePoint lastInvocation = TimePoint::min();
        int& invocations;
        
        Test(ActorRef<Test>, int& invocations_) : invocations(invocations_) { }
        
        void doIt() {
            invocations++;
            ASSERT_GE(std::chrono::time_point_cast<Milliseconds>(Clock::now() - (frequency * .9)).time_since_epoch().count(),
                      std::chrono::time_point_cast<Milliseconds>(lastInvocation).time_since_epoch().count());
            lastInvocation = Clock::now();
        }
    };
    
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    int invocations = 0;
    Actor<Test> test(loop, invocations);
    
    Throttler<Test> throttler { test.self(), frequency };
    
    // Invoke three times in a row
    throttler.invoke(&Test::doIt); // Will be executed immediately
    throttler.invoke(&Test::doIt); // Will be executed delayed
    throttler.invoke(&Test::doIt); // Will not be executed as there is a pending execution
    
    // Let the actor process it's mailbox do it's thing
    auto start = Clock::now();
    while (Clock::now() < start + std::chrono::milliseconds(100)) {
        loop.runOnce();
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
    }
    
    // Should only see the initial invocation
    ASSERT_EQ(invocations, 1);
    
    // Let the runloop process the pending execution
    while (invocations < 2) {
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        loop.runOnce();
    }
    
    // Another invocation should be executed immediately
    throttler.invoke(&Test::doIt);
    while (invocations < 3) {
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        loop.runOnce();
    }
    
}
