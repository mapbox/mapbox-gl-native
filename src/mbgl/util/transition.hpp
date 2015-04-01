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

    inline transition(TimePoint start_, Duration duration_)
        : start(start_),
          duration(duration_) {}

    inline float progress(TimePoint now) const {
        if (duration == Duration::zero()) return 1;
        if (start > now) return 0;

        return std::chrono::duration<float>(now - start) / duration;
    }

    virtual state update(TimePoint now) const = 0;
    virtual ~transition();

protected:
    const TimePoint start;
    const Duration duration;
};

template <typename T>
class ease_transition : public transition {
public:
    ease_transition(T from_, T to_, T& value_, TimePoint start_, Duration duration_)
        : transition(start_, duration_),
          from(from_),
          to(to_),
          value(value_) {}

    state update(TimePoint now) const;

private:
    const T from, to;
    T& value;

};

template <typename T>
class timeout : public transition {
public:
    timeout(T final_value_, T& value_, TimePoint start_, Duration duration_)
        : transition(start_, duration_),
          final_value(final_value_),
          value(value_) {}

    state update(TimePoint now) const {
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

}
}

#endif
