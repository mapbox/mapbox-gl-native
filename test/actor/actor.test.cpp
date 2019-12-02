#include <mbgl/actor/actor.hpp>

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/run_loop.hpp>

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <thread>

using namespace mbgl;
using namespace std::chrono_literals;

TEST(Actor, Construction) {
    struct Test {
        Test(ActorRef<Test>, bool& constructed) {
            constructed = true;
        };
    };

    bool constructed = false;
    Actor<Test> test(Scheduler::GetBackground(), std::ref(constructed));

    EXPECT_TRUE(constructed);
}

TEST(Actor, Destruction) {
    struct Test {
        Test(ActorRef<Test>, bool& destructed_) : destructed(destructed_) {};
        ~Test() {
            destructed = true;
        }

        bool& destructed;
    };

    bool destructed = false;
    {
        Actor<Test> test(Scheduler::GetBackground(), std::ref(destructed));
    }

    EXPECT_TRUE(destructed);
}

TEST(Actor, DestructionBlocksOnReceive) {
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

    std::promise<void> enteredPromise;
    std::future<void> enteredFuture = enteredPromise.get_future();

    std::promise<void> exitingPromise;
    std::future<void> exitingFuture = exitingPromise.get_future();

    Actor<Test> test(Scheduler::GetBackground(), std::move(enteredPromise), std::move(exitingFuture));

    test.self().invoke(&Test::wait);
    enteredFuture.wait();
    exitingPromise.set_value();
}

TEST(Actor, DestructionBlocksOnSend) {
    // Destruction blocks until the actor is not being sent a message.

    struct TestScheduler : public Scheduler {
        std::promise<void> promise;
        std::future<void> future;
        std::atomic<bool> waited;
        mapbox::base::WeakPtrFactory<Scheduler> weakFactory{this};

        TestScheduler(std::promise<void> promise_, std::future<void> future_)
            : promise(std::move(promise_)),
              future(std::move(future_)),
              waited(false) {
        }

        ~TestScheduler() override {
            EXPECT_TRUE(waited.load());
        }

        void schedule(std::function<void()>) final {
            promise.set_value();
            future.wait();
            std::this_thread::sleep_for(1ms);
            waited = true;
        }
        mapbox::base::WeakPtr<Scheduler> makeWeakPtr() override { return weakFactory.makeWeakPtr(); }
    };

    struct Test {
        Test(ActorRef<Test>) {}
        void message() {}
    };

    std::promise<void> enteredPromise;
    std::future<void> enteredFuture = enteredPromise.get_future();

    std::promise<void> exitingPromise;
    std::future<void> exitingFuture = exitingPromise.get_future();

    auto scheduler = std::make_unique<TestScheduler>(std::move(enteredPromise), std::move(exitingFuture));
    auto actor = std::make_unique<Actor<Test>>(*scheduler);

    std::thread thread {
        [] (ActorRef<Test> ref) {
            ref.invoke(&Test::message);
        },
        actor->self()
    };

    enteredFuture.wait();
    exitingPromise.set_value();

    actor.reset();
    scheduler.reset();

    thread.join();
}

TEST(Actor, DestructionAllowedInReceiveOnSameThread) {
    // Destruction doesn't block if occurring on the same
    // thread as receive(). This prevents deadlocks and
    // allows for self-closing actors

    struct Test {

        Test(ActorRef<Test>){};

        void callMeBack(std::function<void ()> callback) {
            callback();
        }
    };

    std::promise<void> callbackFiredPromise;
    std::shared_ptr<Scheduler> retainer = Scheduler::GetBackground();
    auto test = std::make_unique<Actor<Test>>(retainer);

    // Callback (triggered while mutex is locked in Mailbox::receive())
    test->self().invoke(&Test::callMeBack, [&]() {
        // Destroy the Actor/Mailbox in the same thread
        test.reset();
        callbackFiredPromise.set_value();
    });

    auto status = callbackFiredPromise.get_future().wait_for(std::chrono::seconds(1));
    ASSERT_EQ(std::future_status::ready, status);
}

TEST(Actor, SelfDestructionDoesntCrashWaitingReceivingThreads) {
    // Ensures destruction doesn't cause waiting threads to
    // crash when a actor closes it's own mailbox from a
    // callback

    struct Test {

        Test(ActorRef<Test>){};

        void callMeBack(std::function<void ()> callback) {
            callback();
        }
    };


    std::promise<void> actorClosedPromise;

    auto closingActor = std::make_unique<Actor<Test>>(Scheduler::GetBackground());
    auto waitingActor = std::make_unique<Actor<Test>>(Scheduler::GetBackground());

    std::atomic<bool> waitingMessageProcessed {false};

    // Callback (triggered while mutex is locked in Mailbox::receive())
    closingActor->self().invoke(&Test::callMeBack, [&]() {

        // Queue up another message from another thread
        std::promise<void> messageQueuedPromise;
        waitingActor->self().invoke(&Test::callMeBack, [&]() {
            // This will be waiting on the mutex in
            // Mailbox::receive(), holding a lock
            // on the weak_ptr so the mailbox is not
            // destroyed
            closingActor->self().invoke(&Test::callMeBack, [&]() {
                waitingMessageProcessed.store(true);
            });
            messageQueuedPromise.set_value();
        });

        // Wait for the message to be queued
        ASSERT_EQ(
                messageQueuedPromise.get_future().wait_for(std::chrono::seconds(1)),
                std::future_status::ready
        );

        // Destroy the Actor/Mailbox in the same thread
        closingActor.reset();
        actorClosedPromise.set_value();
    });

    auto status = actorClosedPromise.get_future().wait_for(std::chrono::seconds(1));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_FALSE(waitingMessageProcessed.load());
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

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(Scheduler::GetBackground(), std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.self().invoke(&Test::receive, i);
    }

    test.self().invoke(&Test::end);
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

    std::promise<void> endedPromise;
    std::future<void> endedFuture = endedPromise.get_future();
    Actor<Test> test(Scheduler::GetBackground(), std::move(endedPromise));

    for (auto i = 1; i <= 10; ++i) {
        test.self().invoke(&Test::receive, i);
    }

    test.self().invoke(&Test::end);
    endedFuture.wait();
}

TEST(Actor, Ask) {
    // Asking for a result

    struct Test {

        Test(ActorRef<Test>) {}

        int doubleIt(int i) {
            return i * 2;
        }
    };

    Actor<Test> test(Scheduler::GetBackground());

    auto result = test.self().ask(&Test::doubleIt, 1);

    ASSERT_TRUE(result.valid());

    auto status = result.wait_for(std::chrono::seconds(1));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(2, result.get());
}

TEST(Actor, AskVoid) {
    // Ask waits for void methods

    struct Test {
        bool& executed;

        Test(bool& executed_) : executed(executed_) {
        }

        void doIt() {
            executed = true;
        }
    };

    bool executed = false;
    Actor<Test> actor(Scheduler::GetBackground(), executed);

    actor.self().ask(&Test::doIt).get();
    EXPECT_TRUE(executed);
}

TEST(Actor, NoSelfActorRef) {
    // Not all actors need a reference to self

    // Trivially constructable
    struct Trivial {};

    Actor<Trivial> trivial(Scheduler::GetBackground());


    // With arguments
    struct WithArguments {
        std::promise<void> promise;

        WithArguments(std::promise<void> promise_)
        : promise(std::move(promise_)) {
        }

        void receive() {
            promise.set_value();
        }
    };

    std::promise<void> promise;
    auto future = promise.get_future();
    Actor<WithArguments> withArguments(Scheduler::GetBackground(), std::move(promise));

    withArguments.self().invoke(&WithArguments::receive);
    future.wait();
}

TEST(Actor, TwoPhaseConstruction) {
    // This test mimics, in simplified form, the approach used by the Thread<Object> to construct
    // its actor in two parts so that the Thread<Object> instance can be created without waiting
    // for the target thread to be up and running.

    struct Test {
        Test(ActorRef<Test>, std::shared_ptr<bool> destroyed_)
            : destroyed(std::move(destroyed_)) {};

        ~Test() {
            *destroyed = true;
        }

        void callMe(std::promise<void> p) {
            p.set_value();
        }

        void stop() {
            util::RunLoop::Get()->stop();
        }

        std::shared_ptr<bool> destroyed;
    };

    AspiringActor<Test> parent;

    auto destroyed = std::make_shared<bool>(false);

    std::promise<void> queueExecuted;
    auto queueExecutedFuture = queueExecuted.get_future();

    parent.self().invoke(&Test::callMe, std::move(queueExecuted));
    parent.self().invoke(&Test::stop);

    auto thread = std::thread([
        capturedArgs = std::make_tuple(destroyed),
        &parent
    ] () mutable {
        util::RunLoop loop(util::RunLoop::Type::New);
        EstablishedActor<Test> test(loop, parent, capturedArgs);
        loop.run();
    });

    // should not hang
    queueExecutedFuture.get();
    thread.join();

    EXPECT_TRUE(*destroyed);
}


