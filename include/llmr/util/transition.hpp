#ifndef LLMR_UTIL_TRANSITION
#define LLMR_UTIL_TRANSITION

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/time.hpp>

namespace llmr {
namespace util {

class transition : private noncopyable {
public:
    enum state {
        running,
        complete
    };
    transition(float duration)
        : start(util::time()),
          duration(duration) {}

    float progress() const {
        return (util::time() - start) / duration;
    }

    virtual state update() const = 0;
    virtual ~transition();

protected:
    const double start;
    const float duration; // in milliseconds
};

class ease_transition : public transition {
public:
    ease_transition(double from, double to, double& value, float duration);
    state update() const;

private:
    const double from, to;
    double& value;
};

template <typename T>
class timeout : public transition {
public:
    timeout(T final_value, T& value, float duration)
        : transition(duration),
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
