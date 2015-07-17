#include <mbgl/util/transition.hpp>
#include <mbgl/util/unitbezier.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/platform/platform.hpp>

namespace mbgl { namespace util {

UnitBezier ease(0, 0, 0.25, 1);

transition::~transition() {}

template <typename T>
transition::state ease_transition<T>::update(const TimePoint& now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = interpolate(from, to, ease.solve(t, 0.001));
        return running;
    }
}

template class ease_transition<double>;

}}
