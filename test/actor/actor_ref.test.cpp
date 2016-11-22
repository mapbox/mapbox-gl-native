#include <mbgl/actor/actor.hpp>
#include <mbgl/util/default_thread_pool.hpp>

#include <mbgl/test/util.hpp>

#include <chrono>
#include <functional>
#include <future>

using namespace mbgl;
using namespace std::chrono_literals;

TEST(ActorRef, CanOutliveActor) {
    // An ActorRef can outlive its actor. Doing does not extend the actor's lifetime.
    // Sending a message to an ActorRef whose actor has died is a no-op.

    struct Test {
        bool& died;

        Test(ActorRef<Test>, bool& died_)
            : died(died_) {
        }

        ~Test() {
            died = true;
        }

        void receive() {
            FAIL();
        }
    };

    ThreadPool pool { 1 };
    bool died = false;

    ActorRef<Test> test = [&] () {
        return Actor<Test>(pool, std::ref(died)).self();
    }();

    EXPECT_TRUE(died);
    test.invoke(&Test::receive);
}
