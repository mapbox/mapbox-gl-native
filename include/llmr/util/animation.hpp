#ifndef LLMR_UTIL_ANIMATION
#define LLMR_UTIL_ANIMATION

namespace llmr {
namespace util {

class animation {
public:
    enum state {
        running,
        complete
    };

    animation(double from, double to, double &value, double duration);
    animation(const animation &) = delete;
    animation(const animation &&) = delete;
    animation &operator=(const animation &) = delete;

    state update() const;

private:
    const double start, duration;
    const double from, to;
    double &value;
};

}
}

#endif
