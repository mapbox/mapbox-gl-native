#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/threaded_object.hpp>
#include <mbgl/util/thread_local.hpp>

#include <mbgl/test/util.hpp>

#include <future>

using namespace mbgl::util;

namespace {

class TestThread {
public:
    TestThread(mbgl::ActorRef<TestThread>, int *number_) {
        number.set(number_);
    }

    ~TestThread() {
        number.set(nullptr);
    }

    void getNumber(std::promise<int> result){
        result.set_value(*number.get());
    }

private:
    static ThreadLocal<int> number;
};

ThreadLocal<int> TestThread::number;

} // namespace

TEST(ThreadLocalStorage, Basic) {
    RunLoop loop;

    int number1 = 1;
    int number2 = 2;
    int number3 = 3;

    ThreadedObject<TestThread> thread1("Test", &number1);
    ThreadedObject<TestThread> thread2("Test", &number2);
    ThreadedObject<TestThread> thread3("Test", &number3);

    auto thread1Ref = thread1.actor();
    auto thread2Ref = thread2.actor();
    auto thread3Ref = thread3.actor();

    std::promise<int> result1;
    auto result1Future = result1.get_future();
    thread1Ref.invoke(&TestThread::getNumber, std::move(result1));
    EXPECT_EQ(number1, result1Future.get());

    std::promise<int> result2;
    auto result2Future = result2.get_future();
    thread2Ref.invoke(&TestThread::getNumber, std::move(result2));
    EXPECT_EQ(number2, result2Future.get());

    std::promise<int> result3;
    auto result3Future = result3.get_future();
    thread3Ref.invoke(&TestThread::getNumber, std::move(result3));
    EXPECT_EQ(number3, result3Future.get());
}

TEST(ThreadLocalStorage, NotSetReturnsNull) {
    static ThreadLocal<int> number;

    EXPECT_EQ(nullptr, number.get());
}

namespace {

struct DtorCounter {
    ~DtorCounter() { ++(*value); }
    unsigned *value;
};

class TestThreadReclaim {
public:
    TestThreadReclaim(mbgl::ActorRef<TestThreadReclaim>, DtorCounter* counter_) {
        counter.set(counter_);
    }

private:
    static ThreadLocal<DtorCounter> counter;
};

ThreadLocal<DtorCounter> TestThreadReclaim::counter;

} // namespace

TEST(ThreadLocalStorage, AutoReclaim) {
    RunLoop loop;

    unsigned counter = 0;

    auto dtorCounter1 = new DtorCounter{ &counter };
    auto dtorCounter2 = new DtorCounter{ &counter };

    auto thread1 = std::make_unique<ThreadedObject<TestThreadReclaim>>("Test", dtorCounter1);
    auto thread2 = std::make_unique<ThreadedObject<TestThreadReclaim>>("Test", dtorCounter2);

    thread1.reset();
    thread2.reset();

    EXPECT_EQ(counter, 2u);
}
