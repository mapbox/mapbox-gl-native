#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/thread_local.hpp>

#include <mbgl/test/util.hpp>

using namespace mbgl::util;

namespace {

class TestThread {
public:
    TestThread(int *number_) {
        number.set(number_);
    }

    ~TestThread() {
        number.set(nullptr);
    }

    int getNumber() {
        return *number.get();
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

    ThreadContext context = {"Test"};

    Thread<TestThread> thread1(context, &number1);
    Thread<TestThread> thread2(context, &number2);
    Thread<TestThread> thread3(context, &number3);

    EXPECT_EQ(number1, thread1.invokeSync(&TestThread::getNumber));
    EXPECT_EQ(number2, thread2.invokeSync(&TestThread::getNumber));
    EXPECT_EQ(number3, thread3.invokeSync(&TestThread::getNumber));
}

TEST(ThreadLocalStorage, NotSetReturnsNull) {
    static ThreadLocal<int> number;

    EXPECT_EQ(nullptr, number.get());
}

namespace {

class TestThreadReclaim {
public:
    TestThreadReclaim(int* data_) {
        data.set(data_);
    }

private:
    ThreadLocal<int> data;
};

} // namespace

TEST(ThreadLocalStorage, AutoReclaim) {
    RunLoop loop;

    auto data1 = new int;
    auto data2 = new int;

    ThreadContext context = {"Test"};

    auto thread1 = std::make_unique<Thread<TestThreadReclaim>>(context, data1);
    auto thread2 = std::make_unique<Thread<TestThreadReclaim>>(context, data2);

    thread1.reset();
    thread2.reset();

    // Should not leak, valgrind will
    // let us know.
}
