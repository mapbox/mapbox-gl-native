#ifndef MBGL_UTIL_CHANNEL
#define MBGL_UTIL_CHANNEL

#include <mbgl/util/noncopyable.hpp>

#include <mutex>
#include <condition_variable>
#include <queue>

namespace mbgl {

template <class T>
class Channel : public mbgl::util::noncopyable {
public:
    void send(const T& t) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(t);
        condition.notify_one();
    }

    T receive() {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&](){ return !queue.empty(); });

        T t = queue.front();
        queue.pop();

        return t;
    }

private:
    std::mutex mutex;
    std::condition_variable condition;
    std::queue<T> queue;
};

}

#endif
