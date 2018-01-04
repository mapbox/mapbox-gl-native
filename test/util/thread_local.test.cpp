#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
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

    Thread<TestThread> thread1("Test", &number1);
    Thread<TestThread> thread2("Test", &number2);
    Thread<TestThread> thread3("Test", &number3);

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

class TestThreadDataOwnership {
public:
    TestThreadDataOwnership(mbgl::ActorRef<TestThreadDataOwnership>, int* data_) {
        data.set(data_);
    }

    ~TestThreadDataOwnership() {
        data.set(nullptr);
    }

private:
    static ThreadLocal<int> data;
};

ThreadLocal<int> TestThreadDataOwnership::data;

} // namespace

TEST(ThreadLocalStorage, ShouldNotTakeOwnership) {
    RunLoop loop;

    auto data1 = std::make_unique<int>(10);
    auto data2 = std::make_unique<int>(20);

    auto thread1 = std::make_unique<Thread<TestThreadDataOwnership>>("Test", data1.get());
    auto thread2 = std::make_unique<Thread<TestThreadDataOwnership>>("Test", data2.get());

    thread1.reset();
    thread2.reset();

    // Will crash if ThreadLocal destroys
    // the pointer it is managing.
    ASSERT_EQ(*data1, 10);
    ASSERT_EQ(*data2, 20);
}
