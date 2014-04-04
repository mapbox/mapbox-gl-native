#ifndef LLMR_UTIL_ANIMATION
#define LLMR_UTIL_ANIMATION

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/time.hpp>

namespace llmr {
namespace util {

class animation : private noncopyable {
public:
    enum state {
        running,
        complete
    };
    animation(float duration)
        : start(util::time()),
          duration(duration) {}

    float progress() const {
        return (util::time() - start) / duration;
    }

    virtual state update() const = 0;
    virtual ~animation();

protected:
    const double start;
    const float duration; // in milliseconds
};

class ease_animation : public animation {
public:
    ease_animation(double from, double to, double& value, float duration);
    state update() const;

private:
    const double from, to;
    double& value;
};

template <typename T>
class timeout : public animation {
public:
    timeout(T final_value, T& value, float duration)
        : animation(duration),
          final_value(final_value),
          value(value) {}

    state update() const {
        if (progress() >= 1) {
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
