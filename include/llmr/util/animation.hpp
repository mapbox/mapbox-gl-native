#ifndef LLMR_UTIL_ANIMATION
#define LLMR_UTIL_ANIMATION

#include <llmr/util/noncopyable.hpp>

namespace llmr {
namespace util {

class animation : private noncopyable {
public:
    enum state {
        running,
        complete
    };
    animation(double start, double duration)
        : start(start),
          duration(duration) {}

    double progress(double time) const {
        return (time - start) / duration;
    }

    virtual state update(double time) const = 0;
    virtual ~animation();

protected:
    const double start, duration;
};

class ease_animation : public animation {
public:
    ease_animation(double from, double to, double& value, double start, double duration);
    state update(double time) const;

private:
    const double from, to;
    double& value;
};

template <typename T>
class timeout : public animation {
public:
    timeout(T final_value, T& value, double start, double duration)
        : animation(start, duration),
          final_value(final_value),
          value(value) {}

    state update(double time) const {
        if (progress(time) >= 1) {
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
