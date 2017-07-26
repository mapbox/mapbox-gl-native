#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/timer.hpp>

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
            : delegate(delegate_)
            , frequency(frequency_) {
    }
    
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) {
        if (pendingInvocation) {
            return;
        }
        
        Duration timeToNextInvocation = lastInvocation == TimePoint::min()
                                        ? Duration::zero()
                                        : (lastInvocation + frequency) - Clock::now();
        
        if (timeToNextInvocation <= Duration::zero()) {
            lastInvocation = Clock::now();
            delegate.invoke(fn, std::forward<Args>(args)...);
        } else {
            pendingInvocation = true;
            timer.start(timeToNextInvocation, Duration::zero(), [&, fn=std::move(fn), args...] {
                pendingInvocation = false;
                lastInvocation = Clock::now();
                
                delegate.invoke(fn, std::forward<Args>(args)...);
            });
        }
    }

private:
    ActorRef<Delegate> delegate;
    Duration frequency;
    
    bool pendingInvocation = false;
    TimePoint lastInvocation = TimePoint::min();
    
    Timer timer;
};

} // namespace util
} // namespace mbgl
