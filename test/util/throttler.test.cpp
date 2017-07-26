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
        
        Test(std::promise<void> promise_) : promise(std::move(promise_)) {}
        
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
    
    // Throttler needs a RunLoop
    RunLoop loop;
    
    static Milliseconds frequency { 300 };
    
    struct Test {
        TimePoint lastInvocation = TimePoint::min();
        int& invocations;
        
        Test(int& invocations_) : invocations(invocations_) { }
        
        void doIt() {
            invocations++;
            ASSERT_GE((lastInvocation + frequency) - Clock::now(), frequency);
        }
    };
    
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    
    int invocations = 0;
    ThreadPool pool { 2 };
    Actor<Test> test(pool, invocations);
    
    Throttler<Test> throttler { test.self(), frequency };
    
    // Invoke twice in a row
    throttler.invoke(&Test::doIt);
    throttler.invoke(&Test::doIt);
    loop.runOnce(); // Let the timer do it's thing
    ASSERT_EQ(invocations, 1);
    
    // Wait until throttle time passed and invoke again
    std::this_thread::sleep_for (std::chrono::seconds(1));
    throttler.invoke(&Test::doIt);
    loop.runOnce(); // Let the timer do it's thing
    ASSERT_EQ(invocations, 2);
    
}
