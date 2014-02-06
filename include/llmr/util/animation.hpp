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

    animation(double from, double to, double &value, double duration);
    state update() const;

private:
    const double start, duration;
    const double from, to;
    double &value;
};

}
}

#endif
