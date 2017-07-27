#include <mbgl/actor/scheduler.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/actor/message.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {
namespace util {

/**
 * Throttles invocations to the Delegate Actor, throttling is done
 * over all methods, not on a per method base.
 */
template <class Delegate>
class Throttler {

public:
    Throttler(ActorRef<Delegate> delegate_, Duration frequency_)
            : delegate(std::move(delegate_))
            , frequency(frequency_) {
    }
    
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        if (pendingInvocation && !pendingInvocation->isFinished()) {
            return;
        }
        
        Duration timeToNextInvocation = lastInvocation == TimePoint::min()
                                        ? Duration::zero()
                                        : (lastInvocation + frequency) - Clock::now();
        
        if (timeToNextInvocation <= Duration::zero()) {
            lastInvocation = Clock::now();
            delegate.invoke(fn, std::forward<Args>(args)...);
        } else {
            pendingInvocation = delegate.invokeDelayed(timeToNextInvocation, fn, std::forward<Args>(args)...);
            lastInvocation = Clock::now() + timeToNextInvocation;
        }
    }

private:
    ActorRef<Delegate> delegate;
    Duration frequency;
    
    std::unique_ptr<Scheduler::Scheduled> pendingInvocation;
    TimePoint lastInvocation = TimePoint::min();
};

} // namespace util
} // namespace mbgl
