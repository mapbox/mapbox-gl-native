#include <mbgl/actor/actor.hpp>
#include <mbgl/util/default_thread_pool.hpp>

#include <mbgl/test/util.hpp>

#include <future>

using namespace mbgl;

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

TEST(ActorRef, Ask) {
    // Ask returns a Future eventually returning the result

    struct Test {

        Test(ActorRef<Test>) {}

        int gimme() {
            return 20;
        }

        int echo(int i) {
            return i;
        }
    };

    ThreadPool pool { 1 };
    Actor<Test> actor(pool);
    ActorRef<Test> ref = actor.self();

    EXPECT_EQ(20, ref.ask(&Test::gimme).get());
    EXPECT_EQ(30, ref.ask(&Test::echo, 30).get());
}

TEST(ActorRef, AskVoid) {
    // Ask waits for void methods
    
    struct Test {
        bool& executed;
        
        Test(bool& executed_) : executed(executed_) {
        }
        
        void doIt() {
            executed = true;
        }
    };
    
    ThreadPool pool { 1 };
    bool executed = false;
    Actor<Test> actor(pool, executed);
    ActorRef<Test> ref = actor.self();
    
    ref.ask(&Test::doIt).get();
    EXPECT_TRUE(executed);
}

TEST(ActorRef, AskOnDestroyedActor) {
    // Tests behavior when calling ask() after the
    // Actor has gone away. Should set a exception_ptr.

    struct Test {
        bool& died;

        Test(ActorRef<Test>, bool& died_) : died(died_) {}

        ~Test() {
            died = true;
        }

        int receive() {
            return 1;
        }
    };
    bool died = false;

    ThreadPool pool { 1 };
    auto actor = std::make_unique<Actor<Test>>(pool, died);
    ActorRef<Test> ref = actor->self();

    actor.reset();
    EXPECT_TRUE(died);

    auto result = ref.ask(&Test::receive);
    EXPECT_ANY_THROW(result.get());
}
