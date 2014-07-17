#ifndef MBGL_UTIL_TRANSITION
#define MBGL_UTIL_TRANSITION

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {
namespace util {

class transition : private noncopyable {
public:
    enum state {
        running,
        complete
    };

    inline transition(timestamp start, timestamp duration)
        : start(start),
          duration(duration) {}

    inline float progress(timestamp now) const {
        if (duration == 0) return 1;
        if (start > now) return 0;

        return (float)(now - start) / duration;
    }

    virtual state update(timestamp now) const = 0;
    virtual ~transition();

protected:
    double interpolateDouble(double from, double to, double t) const;
    float interpolateFloat(float from, float to, double t) const;
    Color interpolateColor(Color from, Color to, double t) const;
    std::array<float, 2> interpolateFloatArray(std::array<float, 2> from, std::array<float, 2> to, double t) const;

protected:
    const timestamp start, duration;
};

template <typename T>
class ease_transition : public transition {
public:
    ease_transition(T from, T to, T& value, timestamp start, timestamp duration)
        : transition(start, duration),
          from(from),
          to(to),
          value(value) {}

    state update(timestamp now) const;

private:
    const T from, to;
    T& value;

};

template <typename T>
class timeout : public transition {
public:
    timeout(T final_value, T& value, timestamp start, timestamp duration)
        : transition(start, duration),
          final_value(final_value),
          value(value) {}

    state update(timestamp now) const {
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
