#include <llmr/util/animation.hpp>
#include <llmr/util/unitbezier.hpp>
#include <llmr/platform/platform.hpp>

using namespace llmr::util;

UnitBezier ease(0.25, 0.1, 0.25, 1);

animation::~animation() {}

ease_animation::ease_animation(double from, double to, double &value, double duration)
    : animation(duration),
      from(from),
      to(to),
      value(value) {
}

animation::state ease_animation::update() const {
    double t = progress();
    if (t >= 1) {
        value = to;
        return complete;
    } else {
        value = from + (to - from) * ease.solve(t, 0.001);
        return running;
    }
}
