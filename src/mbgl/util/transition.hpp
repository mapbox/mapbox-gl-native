#ifndef MBGL_UTIL_TRANSITION
#define MBGL_UTIL_TRANSITION

#include <mbgl/util/noncopyable.hpp>
#include <chrono>

namespace mbgl {
namespace util {

class transition : private noncopyable {
public:
    enum state {
        running,
        complete
    };

    inline transition(std::chrono::steady_clock::time_point start_, std::chrono::steady_clock::duration duration_)
        : start(start_),
          duration(duration_) {}

    inline float progress(std::chrono::steady_clock::time_point now) const {
        if (duration == std::chrono::steady_clock::duration::zero()) return 1;
        if (start > now) return 0;

        return std::chrono::duration<float>(now - start) / duration;
    }

    virtual state update(std::chrono::steady_clock::time_point now) const = 0;
    virtual ~transition();

protected:
    const std::chrono::steady_clock::time_point start;
    const std::chrono::steady_clock::duration duration;
};

template <typename T>
class ease_transition : public transition {
public:
    ease_transition(T from_, T to_, T& value_, std::chrono::steady_clock::time_point start_, std::chrono::steady_clock::duration duration_)
        : transition(start_, duration_),
          from(from_),
          to(to_),
          value(value_) {}

    state update(std::chrono::steady_clock::time_point now) const;

private:
    const T from, to;
    T& value;

};

template <typename T>
class timeout : public transition {
public:
    timeout(T final_value_, T& value_, std::chrono::steady_clock::time_point start_, std::chrono::steady_clock::duration duration_)
        : transition(start_, duration_),
          final_value(final_value_),
          value(value_) {}

    state update(std::chrono::steady_clock::time_point now) const {
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
