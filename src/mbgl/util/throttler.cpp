#include <mbgl/util/throttler.hpp>

namespace mbgl {
namespace util {

Throttler::Throttler(Duration frequency_, std::function<void()>&& function_)
    : frequency(frequency_)
    , function(std::move(function_))
    , pendingInvocation(false)
    , lastInvocation(TimePoint::min())
{}

void Throttler::invoke() {
    if (pendingInvocation) {
        return;
    }
    
    Duration timeToNextInvocation = lastInvocation == TimePoint::min()
        ? Duration::zero()
        : (lastInvocation + frequency) - Clock::now();
    
    if (timeToNextInvocation <= Duration::zero()) {
        lastInvocation = Clock::now();
        function();
    } else {
        pendingInvocation = true;
        timer.start(timeToNextInvocation, Duration::zero(), [this]{
            pendingInvocation = false;
            lastInvocation = Clock::now();
            function();
        });
    }
}

} // namespace util
} // namespace mbgl
