#include <mbgl/util/chrono.hpp>
#include <mbgl/util/timer.hpp>

namespace mbgl {
namespace util {

class Throttler {
public:
    Throttler(Duration frequency, std::function<void()>&& function);
    
    void invoke();
private:
    Duration frequency;
    std::function<void()> function;
    
    Timer timer;
    bool pendingInvocation;
    TimePoint lastInvocation;
};

} // namespace util
} // namespace mbgl
