#ifndef MBGL_UTIL_TRANSITION
#define MBGL_UTIL_TRANSITION

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {
namespace util {

class transition : private noncopyable {
public:
    enum state {
        running,
        complete
    };

    inline transition(const TimePoint& start_, const Duration& duration_)
        : start(start_),
          duration(duration_) {}

    inline float progress(const TimePoint& now) const {
        if (duration == Duration::zero()) return 1;
        if (start > now) return 0;

        return std::chrono::duration<float>(now - start) / duration;
    }

    virtual state update(const TimePoint& now) const = 0;
    virtual ~transition();

protected:
    const TimePoint start;
    const Duration duration;
};

template <typename T>
class ease_transition final : public transition {
public:
    ease_transition(T from_, T to_, T& value_, const TimePoint& start_, const Duration& duration_)
        : transition(start_, duration_),
          from(from_),
          to(to_),
          value(value_) {}

    state update(const TimePoint& now) const override;

private:
    const T from, to;
    T& value;

};

template <typename T>
class timeout final : public transition {
public:
    timeout(T final_value_, T& value_, const TimePoint& start_, const Duration& duration_)
        : transition(start_, duration_),
          final_value(final_value_),
          value(value_) {}

    state update(const TimePoint& now) const override {
        if (progress(now) >= 1) {
            value = final_value;
            return complete;
        } else {
            return running;
        }
    }

private:
    const T final_value;
    T& value;
};

} // namespace util
} // namespace mbgl

#endif
