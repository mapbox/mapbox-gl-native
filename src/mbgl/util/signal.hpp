#ifndef MBGL_UTIL_SIGNAL
#define MBGL_UTIL_SIGNAL

#include <mutex>
#include <condition_variable>

namespace mbgl {
namespace util {

class Signal {
public:
    void wait();
    void notify();

private:
    bool status = false;
    std::condition_variable condition;
    std::mutex mutex;
};

}
}

#endif
