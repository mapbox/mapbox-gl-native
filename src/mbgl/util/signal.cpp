#include "signal.hpp"

namespace mbgl {
namespace util {

void Signal::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [this] { return status; });
    status = false;
}

void Signal::notify() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        status = true;
    }
    condition.notify_all();
}

}
}
