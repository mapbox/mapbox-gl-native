#include <llmr/util/transition.hpp>
#include <llmr/util/unitbezier.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::util;

UnitBezier ease(0.25, 0.1, 0.25, 1);

transition::~transition() {}

ease_transition::ease_transition(double from, double to, double &value, time start, time duration)
    : transition(start, duration),
      from(from),
      to(to),
      value(value) {
}

transition::state ease_transition::update(time now) const {
    float t = progress(now);
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = from + (to - from) * ease.solve(t, 0.001);
        return running;
    }
}
